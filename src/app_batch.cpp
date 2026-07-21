#include "app.hpp"
#include "business/contract_batch.hpp"
#include "html/format.hpp"
#include "html/layout.hpp"
#include "pages/contracts_pages.hpp"

void App::handle_batch_new_form(const httplib::Request &req, httplib::Response &res) {
    if (!req.has_param("tenant_id")) {
        not_found(res, "Не указан арендатор");
        return;
    }
    int64_t tenant_id = std::stoll(req.get_param_value("tenant_id"));
    auto tenant = m_tenants.get(tenant_id);
    if (!tenant) {
        not_found(res, "Арендатор не найден");
        return;
    }

    std::string start_date = req.has_param("start_date") ? req.get_param_value("start_date") : "";
    std::string rent_amount = req.has_param("rent_amount") ? req.get_param_value("rent_amount") : "";

    std::vector<contract_batch::BatchPeriod> periods;
    std::optional<std::string> error;
    if (!start_date.empty() && !rent_amount.empty()) {
        try {
            auto start = fmt_util::parse_iso_date(start_date);
            long long rent_kopecks = fmt_util::parse_amount_to_kopecks(rent_amount);
            if (rent_kopecks <= 0) {
                error = "Сумма аренды должна быть больше нуля";
            } else {
                periods = contract_batch::split_rent_period_into_batch(start, rent_kopecks);
            }
        } catch (const std::exception &) {
            error = "Некорректная дата начала периода";
        }
    }

    std::string body = pages::build_batch_form_page(*tenant, start_date, rent_amount, periods, error);
    res.set_content(layout::page_shell("Пачка договоров", "tenants", body), "text/html; charset=utf-8");
}

void App::handle_batch_create(const httplib::Request &req, httplib::Response &res) {
    int64_t tenant_id = std::stoll(req.get_param_value("tenant_id"));
    auto tenant = m_tenants.get(tenant_id);
    if (!tenant) {
        not_found(res, "Арендатор не найден");
        return;
    }

    auto start = fmt_util::parse_iso_date(req.get_param_value("start_date"));
    long long rent_kopecks = fmt_util::parse_amount_to_kopecks(req.get_param_value("rent_amount"));
    auto periods = contract_batch::split_rent_period_into_batch(start, rent_kopecks);

    SQLite::Transaction transaction(m_db.handle());

    std::vector<int64_t> ids;
    ids.reserve(periods.size());
    for (const auto &p : periods) {
        Contract c;
        c.tenant_id = tenant_id;
        c.rent_kopecks = rent_kopecks;
        c.start_date = fmt_util::format_iso_date(p.start);
        c.end_date = fmt_util::format_iso_date(p.end);
        c.is_signed = false;
        ids.push_back(m_contracts.create(c));
    }

    if (ids.size() > 1) {
        int64_t batch_id = ids.front();
        int64_t seq = 1;
        for (int64_t id : ids) {
            m_contracts.set_batch(id, batch_id, seq++);
        }
    }

    transaction.commit();

    res.set_redirect("/contracts?tenant_id=" + std::to_string(tenant_id));
}
