#include "app.hpp"
#include "html/format.hpp"
#include "html/layout.hpp"
#include "pages/contracts_pages.hpp"

namespace {
std::vector<pages::ContractRow> join_tenants(const ContractRepository &contracts_repo, const TenantRepository &tenants_repo,
                                              const ContractFilter &filter) {
    std::vector<pages::ContractRow> rows;
    for (auto &c : contracts_repo.list(filter)) {
        auto tenant = tenants_repo.get(c.tenant_id);
        if (!tenant) continue;
        rows.push_back(pages::ContractRow{c, *tenant});
    }
    return rows;
}
}  // namespace

void App::handle_contracts_list(const httplib::Request &req, httplib::Response &res) {
    ContractFilter filter;
    pages::ContractsListFilter page_filter;
    if (req.has_param("tenant_id") && !req.get_param_value("tenant_id").empty()) {
        int64_t tenant_id = std::stoll(req.get_param_value("tenant_id"));
        filter.tenant_id = tenant_id;
        page_filter.tenant_id = tenant_id;
    }
    if (req.has_param("signed") && !req.get_param_value("signed").empty()) {
        bool signed_ = req.get_param_value("signed") == "1";
        filter.is_signed = signed_;
        page_filter.is_signed = signed_;
    }

    std::optional<std::string> error;
    if (req.has_param("error")) error = req.get_param_value("error");

    auto rows = join_tenants(m_contracts, m_tenants, filter);
    std::string body = pages::build_contracts_list_page(rows, m_tenants.list(), page_filter, error);
    res.set_content(layout::page_shell("Договоры", "contracts", body), "text/html; charset=utf-8");
}

void App::handle_contracts_add_form(const httplib::Request &req, httplib::Response &res) {
    Contract c;
    if (req.has_param("tenant_id") && !req.get_param_value("tenant_id").empty()) {
        c.tenant_id = std::stoll(req.get_param_value("tenant_id"));
    }
    std::string body = pages::build_contract_form_page(c, m_tenants.list(), false, std::nullopt);
    res.set_content(layout::page_shell("Добавление договора", "contracts", body), "text/html; charset=utf-8");
}

void App::handle_contracts_add(const httplib::Request &req, httplib::Response &res) {
    Contract c;
    c.tenant_id = std::stoll(req.get_param_value("tenant_id"));
    c.rent_kopecks = fmt_util::parse_amount_to_kopecks(req.get_param_value("rent_amount"));
    c.start_date = req.get_param_value("start_date");
    c.end_date = req.get_param_value("end_date");
    c.is_signed = false;
    m_contracts.create(c);
    res.set_redirect("/contracts?tenant_id=" + std::to_string(c.tenant_id));
}

void App::handle_contracts_edit_form(const httplib::Request &req, httplib::Response &res) {
    if (!req.has_param("id")) {
        not_found(res, "Договор не найден");
        return;
    }
    int64_t id = std::stoll(req.get_param_value("id"));
    auto contract = m_contracts.get(id);
    if (!contract) {
        not_found(res, "Договор не найден");
        return;
    }
    std::string body = pages::build_contract_form_page(*contract, m_tenants.list(), true, std::nullopt);
    res.set_content(layout::page_shell("Редактирование договора", "contracts", body), "text/html; charset=utf-8");
}

void App::handle_contracts_update(const httplib::Request &req, httplib::Response &res) {
    if (!req.has_param("id")) {
        not_found(res, "Договор не найден");
        return;
    }
    int64_t id = std::stoll(req.get_param_value("id"));
    Contract c;
    c.id = id;
    c.tenant_id = std::stoll(req.get_param_value("tenant_id"));
    c.rent_kopecks = fmt_util::parse_amount_to_kopecks(req.get_param_value("rent_amount"));
    c.start_date = req.get_param_value("start_date");
    c.end_date = req.get_param_value("end_date");
    m_contracts.update(c);
    m_contracts.set_signed(id, req.has_param("is_signed"));
    res.set_redirect("/contracts?tenant_id=" + std::to_string(c.tenant_id));
}

void App::handle_contracts_delete(const httplib::Request &req, httplib::Response &res) {
    if (!req.has_param("id")) {
        not_found(res, "Договор не найден");
        return;
    }
    int64_t id = std::stoll(req.get_param_value("id"));
    m_contracts.remove(id);
    res.set_redirect("/contracts");
}

void App::handle_contracts_toggle_signed(const httplib::Request &req, httplib::Response &res) {
    if (!req.has_param("id")) {
        not_found(res, "Договор не найден");
        return;
    }
    int64_t id = std::stoll(req.get_param_value("id"));
    auto contract = m_contracts.get(id);
    if (!contract) {
        not_found(res, "Договор не найден");
        return;
    }
    m_contracts.set_signed(id, !contract->is_signed);

    std::string redirect = req.has_param("redirect") ? req.get_param_value("redirect") : "/contracts";
    res.set_redirect(redirect);
}
