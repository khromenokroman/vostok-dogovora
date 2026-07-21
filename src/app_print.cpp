#include "app.hpp"
#include "pages/print_pages.hpp"

namespace {
std::vector<std::pair<Contract, Tenant>> to_print_items(const std::vector<Contract> &contracts,
                                                          const TenantRepository &tenants_repo) {
    std::vector<std::pair<Contract, Tenant>> items;
    items.reserve(contracts.size());
    for (const auto &c : contracts) {
        auto tenant = tenants_repo.get(c.tenant_id);
        if (!tenant) continue;
        items.emplace_back(c, *tenant);
    }
    return items;
}
}  // namespace

void App::handle_print_contract(const httplib::Request &req, httplib::Response &res) {
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
    auto tenant = m_tenants.get(contract->tenant_id);
    if (!tenant) {
        not_found(res, "Арендатор не найден");
        return;
    }

    std::vector<std::pair<Contract, Tenant>> items{{*contract, *tenant}};
    std::string html = pages::build_print_document("Договор аренды", items, m_settings.get());
    res.set_content(html, "text/html; charset=utf-8");
}

void App::handle_print_batch(const httplib::Request &req, httplib::Response &res) {
    if (!req.has_param("batch_id")) {
        not_found(res, "Пачка не найдена");
        return;
    }
    int64_t batch_id = std::stoll(req.get_param_value("batch_id"));
    auto contracts = m_contracts.list_by_batch(batch_id);
    if (contracts.empty()) {
        not_found(res, "Пачка не найдена");
        return;
    }

    auto items = to_print_items(contracts, m_tenants);
    std::string html = pages::build_print_document("Пачка договоров аренды", items, m_settings.get());
    res.set_content(html, "text/html; charset=utf-8");
}

void App::handle_print_selected(const httplib::Request &req, httplib::Response &res) {
    std::vector<int64_t> ids;
    auto range = req.params.equal_range("ids");
    for (auto it = range.first; it != range.second; ++it) {
        try {
            ids.push_back(std::stoll(it->second));
        } catch (const std::exception &) {
            // игнорируем некорректные значения
        }
    }

    auto contracts = m_contracts.list_by_ids(ids);
    auto items = to_print_items(contracts, m_tenants);
    std::string html = pages::build_print_document("Выбранные договоры аренды", items, m_settings.get());
    res.set_content(html, "text/html; charset=utf-8");
}
