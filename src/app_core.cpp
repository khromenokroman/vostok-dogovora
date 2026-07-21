#include <filesystem>
#include <iostream>
#include <sstream>

#include "app.hpp"
#include "html/format.hpp"
#include "html/layout.hpp"

namespace {
std::string ensure_parent_dir(const std::string &path) {
    std::filesystem::path p(path);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }
    return path;
}
}  // namespace

App::App(Config config)
    : m_config(std::move(config)),
      m_db(ensure_parent_dir(m_config.db_path)),
      m_tenants(m_db.handle()),
      m_contracts(m_db.handle()),
      m_settings(m_db.handle()) {
    register_routes();
}

void App::not_found(httplib::Response &res, const std::string &message) {
    res.status = 404;
    res.set_content(message, "text/plain; charset=utf-8");
}

void App::register_routes() {
    m_server.Get("/", [this](const httplib::Request &req, httplib::Response &res) { handle_dashboard(req, res); });

    m_server.Get("/favicon.ico", [](const httplib::Request &, httplib::Response &res) { res.status = 204; });

    m_server.Get("/tenants", [this](const httplib::Request &req, httplib::Response &res) { handle_tenants_list(req, res); });
    m_server.Get("/tenants/add", [this](const httplib::Request &req, httplib::Response &res) { handle_tenants_add_form(req, res); });
    m_server.Post("/tenants/add", [this](const httplib::Request &req, httplib::Response &res) { handle_tenants_add(req, res); });
    m_server.Get("/tenants/edit", [this](const httplib::Request &req, httplib::Response &res) { handle_tenants_edit_form(req, res); });
    m_server.Post("/tenants/update", [this](const httplib::Request &req, httplib::Response &res) { handle_tenants_update(req, res); });
    m_server.Get("/tenants/delete", [this](const httplib::Request &req, httplib::Response &res) { handle_tenants_delete(req, res); });

    m_server.Get("/contracts", [this](const httplib::Request &req, httplib::Response &res) { handle_contracts_list(req, res); });
    m_server.Get("/contracts/add", [this](const httplib::Request &req, httplib::Response &res) { handle_contracts_add_form(req, res); });
    m_server.Post("/contracts/add", [this](const httplib::Request &req, httplib::Response &res) { handle_contracts_add(req, res); });
    m_server.Get("/contracts/edit", [this](const httplib::Request &req, httplib::Response &res) { handle_contracts_edit_form(req, res); });
    m_server.Post("/contracts/update", [this](const httplib::Request &req, httplib::Response &res) { handle_contracts_update(req, res); });
    m_server.Get("/contracts/delete", [this](const httplib::Request &req, httplib::Response &res) { handle_contracts_delete(req, res); });
    m_server.Get("/contracts/toggle-signed", [this](const httplib::Request &req, httplib::Response &res) { handle_contracts_toggle_signed(req, res); });

    m_server.Get("/contracts/batch/new", [this](const httplib::Request &req, httplib::Response &res) { handle_batch_new_form(req, res); });
    m_server.Post("/contracts/batch/create", [this](const httplib::Request &req, httplib::Response &res) { handle_batch_create(req, res); });

    m_server.Get("/print/contract", [this](const httplib::Request &req, httplib::Response &res) { handle_print_contract(req, res); });
    m_server.Get("/print/batch", [this](const httplib::Request &req, httplib::Response &res) { handle_print_batch(req, res); });
    m_server.Post("/print/selected", [this](const httplib::Request &req, httplib::Response &res) { handle_print_selected(req, res); });

    m_server.Get("/settings", [this](const httplib::Request &req, httplib::Response &res) { handle_settings_form(req, res); });
    m_server.Post("/settings/update", [this](const httplib::Request &req, httplib::Response &res) { handle_settings_update(req, res); });
}

void App::run() {
    std::cout << "Сервер запущен на http://" << m_config.host << ":" << m_config.port << std::endl;
    m_server.listen(m_config.host, m_config.port);
}

void App::handle_dashboard(const httplib::Request &, httplib::Response &res) {
    int64_t tenants_count = static_cast<int64_t>(m_tenants.list().size());
    int64_t contracts_total = m_contracts.count_total();
    int64_t contracts_unsigned = m_contracts.count_unsigned();

    std::ostringstream body;
    body << R"(<div class="header"><div><h1>Восток — учёт аренды</h1>
<p class="subtitle">Арендаторы, договоры и печать документов универмага «Восток».</p></div></div>
<div class="stat-grid">
<div class="stat-card"><div class="value">)"
         << tenants_count << R"(</div><div class="label">Арендаторов</div></div>
<div class="stat-card"><div class="value">)"
         << contracts_total << R"(</div><div class="label">Договоров всего</div></div>
<div class="stat-card)"
         << (contracts_unsigned > 0 ? " alert" : "") << R"("><div class="value">)"
         << contracts_unsigned << R"(</div><div class="label">Не подписано</div></div>
</div>
<div class="header-actions">
<a class="header-btn primary" href="/tenants/add">Добавить арендатора</a>
<a class="header-btn batch" href="/tenants">Создать пачку договоров</a>
<a class="header-btn print-all" href="/contracts?signed=0">Кто не подписал</a>
<a class="header-btn settings" href="/settings">Настройки арендодателя</a>
</div>)";

    res.set_content(layout::page_shell("Восток — учёт аренды", "dashboard", body.str()), "text/html; charset=utf-8");
}
