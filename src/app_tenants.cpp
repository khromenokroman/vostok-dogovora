#include "app.hpp"
#include "html/format.hpp"
#include "html/layout.hpp"
#include "pages/tenants_pages.hpp"

namespace {
double parse_double_or(const std::string &s, double fallback) {
    try {
        return std::stod(s);
    } catch (...) {
        return fallback;
    }
}
}  // namespace

Tenant App::tenant_from_form(const httplib::Request &req, int64_t id) const {
    Tenant t;
    t.id = id;
    t.fio = req.get_param_value("fio");
    t.activity_basis = req.get_param_value("activity_basis");
    t.address = req.get_param_value("address");
    t.inn = req.get_param_value("inn");
    t.ogrn = req.get_param_value("ogrn");
    t.passport_data = req.get_param_value("passport_data");
    t.phone = req.get_param_value("phone");
    t.email = req.get_param_value("email");
    t.room_number = req.get_param_value("room_number");
    t.area_total_sqm = parse_double_or(req.get_param_value("area_total_sqm"), 0.0);
    t.area_trade_sqm = parse_double_or(req.get_param_value("area_trade_sqm"), 0.0);
    t.notes = req.get_param_value("notes");
    return t;
}

void App::handle_tenants_list(const httplib::Request &req, httplib::Response &res) {
    std::optional<std::string> error;
    if (req.has_param("error")) error = req.get_param_value("error");

    std::string body = pages::build_tenants_list_page(m_tenants.list(), error);
    res.set_content(layout::page_shell("Арендаторы", "tenants", body), "text/html; charset=utf-8");
}

void App::handle_tenants_add_form(const httplib::Request &, httplib::Response &res) {
    std::string body = pages::build_tenant_form_page(Tenant{}, false, std::nullopt);
    res.set_content(layout::page_shell("Добавление арендатора", "tenants", body), "text/html; charset=utf-8");
}

void App::handle_tenants_add(const httplib::Request &req, httplib::Response &res) {
    Tenant t = tenant_from_form(req, 0);
    m_tenants.create(t);
    res.set_redirect("/tenants");
}

void App::handle_tenants_edit_form(const httplib::Request &req, httplib::Response &res) {
    if (!req.has_param("id")) {
        not_found(res, "Арендатор не найден");
        return;
    }
    int64_t id = std::stoll(req.get_param_value("id"));
    auto tenant = m_tenants.get(id);
    if (!tenant) {
        not_found(res, "Арендатор не найден");
        return;
    }
    std::string body = pages::build_tenant_form_page(*tenant, true, std::nullopt);
    res.set_content(layout::page_shell("Редактирование арендатора", "tenants", body), "text/html; charset=utf-8");
}

void App::handle_tenants_update(const httplib::Request &req, httplib::Response &res) {
    if (!req.has_param("id")) {
        not_found(res, "Арендатор не найден");
        return;
    }
    int64_t id = std::stoll(req.get_param_value("id"));
    Tenant t = tenant_from_form(req, id);
    m_tenants.update(t);
    res.set_redirect("/tenants");
}

void App::handle_tenants_delete(const httplib::Request &req, httplib::Response &res) {
    if (!req.has_param("id")) {
        not_found(res, "Арендатор не найден");
        return;
    }
    int64_t id = std::stoll(req.get_param_value("id"));

    if (m_tenants.has_contracts(id)) {
        ContractFilter filter;
        filter.tenant_id = id;
        size_t count = m_contracts.list(filter).size();
        std::string msg = "Удаление отменено: у арендатора есть " + std::to_string(count) +
                           " договор(ов) — сначала удалите их на странице «Договоры»";
        res.set_redirect("/tenants?error=" + fmt_util::url_encode(msg));
        return;
    }

    bool ok = m_tenants.remove(id);
    if (!ok) {
        res.set_redirect("/tenants?error=" + fmt_util::url_encode("Не удалось удалить арендатора"));
        return;
    }
    res.set_redirect("/tenants");
}
