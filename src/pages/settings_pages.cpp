#include "settings_pages.hpp"

#include <sstream>

#include "html/format.hpp"

namespace pages {

using fmt_util::html_escape;

namespace {
void field(std::ostringstream &body, const char *label, const char *name, const std::string &value,
           const char *placeholder = "") {
    body << R"(<div class="field"><label>)" << label << R"(</label><input name=")" << name << R"(" placeholder=")"
         << placeholder << R"(" value=")" << html_escape(value) << R"("></div>)";
}
}  // namespace

std::string build_settings_page(const OrganizationSettings &s, std::optional<std::string> notice) {
    std::ostringstream body;
    body << R"(<h1>Настройки арендодателя</h1>
<p class="subtitle">Эти реквизиты подставляются во все печатаемые договоры.</p>)";

    if (notice) {
        body << R"(<div class="banner notice">)" << html_escape(*notice) << "</div>";
    }

    body << R"(<form action="/settings/update" method="post">
<div class="doc-section-title" style="margin-top:0;">Организация-арендодатель</div>
<div class="pair-grid">)";
    field(body, "Наименование", "org_name", s.org_name, "Общество с ограниченной ответственностью «ПРОДКООП»");
    field(body, "ФИО директора", "director_fio", s.director_fio, "Пузыревский Виктор Вацлавович");
    field(body, "Основание полномочий директора", "director_basis", s.director_basis, "Устава");
    field(body, "ИНН", "inn", s.inn, "3902007063");
    field(body, "КПП", "kpp", s.kpp, "390201001");
    field(body, "Юридический адрес", "legal_address", s.legal_address,
          "238050, Россия, Калининградская область, г. Гусев, ул. Победы, 29");
    field(body, "Банк", "bank_name", s.bank_name, "Калининградское ОСБ № 8626 г. Калининград");
    field(body, "Расчётный счёт", "bank_account", s.bank_account, "40702810020070000008");
    field(body, "Корр. счёт", "bank_corr_account", s.bank_corr_account, "30101810100000000634");
    field(body, "БИК", "bank_bik", s.bank_bik, "042748634");
    body << "</div>";

    body << R"(<div class="doc-section-title">Универмаг</div>
<div class="pair-grid">)";
    field(body, "Название универмага", "mall_name", s.mall_name, "Восток");
    field(body, "Город", "mall_city", s.mall_city, "г. Гусев");
    field(body, "Адрес", "mall_address", s.mall_address, "Калининградская обл., г. Гусев, ул. Победы, 29");
    field(body, "Этаж (по умолчанию)", "mall_floor", s.mall_floor, "2");
    body << "</div>";

    body << R"(<div class="actions-row">
<button class="btn" type="submit">Сохранить</button>
<a class="back" href="/">Отмена</a>
</div></form>)";

    return body.str();
}

}  // namespace pages
