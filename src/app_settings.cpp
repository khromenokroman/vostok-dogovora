#include "app.hpp"
#include "html/layout.hpp"
#include "pages/settings_pages.hpp"

void App::handle_settings_form(const httplib::Request &req, httplib::Response &res) {
    std::optional<std::string> notice;
    if (req.has_param("saved")) notice = "Настройки сохранены";

    std::string body = pages::build_settings_page(m_settings.get(), notice);
    res.set_content(layout::page_shell("Настройки", "settings", body), "text/html; charset=utf-8");
}

void App::handle_settings_update(const httplib::Request &req, httplib::Response &res) {
    OrganizationSettings s;
    s.org_name = req.get_param_value("org_name");
    s.director_fio = req.get_param_value("director_fio");
    s.director_basis = req.get_param_value("director_basis");
    s.inn = req.get_param_value("inn");
    s.kpp = req.get_param_value("kpp");
    s.legal_address = req.get_param_value("legal_address");
    s.bank_name = req.get_param_value("bank_name");
    s.bank_account = req.get_param_value("bank_account");
    s.bank_corr_account = req.get_param_value("bank_corr_account");
    s.bank_bik = req.get_param_value("bank_bik");
    s.mall_name = req.get_param_value("mall_name");
    s.mall_city = req.get_param_value("mall_city");
    s.mall_address = req.get_param_value("mall_address");
    s.mall_floor = req.get_param_value("mall_floor");

    m_settings.upsert(s);
    res.set_redirect("/settings?saved=1");
}
