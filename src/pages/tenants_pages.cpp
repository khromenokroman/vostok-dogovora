#include "tenants_pages.hpp"

#include <sstream>

#include "html/format.hpp"

namespace pages {

using fmt_util::html_escape;

std::string build_tenants_list_page(const std::vector<Tenant> &tenants, std::optional<std::string> error) {
    std::ostringstream body;
    body << R"(<div class="header"><div><h1>Арендаторы</h1>
<p class="subtitle">Список арендаторов, их реквизиты и торговые места.</p></div>
<div class="header-actions">
<a class="header-btn primary" href="/tenants/add">Добавить арендатора</a>
</div></div>)";

    if (error) {
        body << R"(<div class="banner error">)" << html_escape(*error) << "</div>";
    }

    body << R"(<div class="table-wrap"><table><thead><tr>
<th>ФИО / наименование</th>
<th>ИНН</th>
<th>Телефон</th>
<th>Помещение</th>
<th>Площадь, м²</th>
<th>Торг. площадь, м²</th>
<th>Действия</th>
</tr></thead><tbody>)";

    if (tenants.empty()) {
        body << R"(<tr><td class="empty" colspan="7">Список арендаторов пока пуст.</td></tr>)";
    } else {
        for (const auto &t : tenants) {
            body << "<tr>";
            body << R"(<td class="text-cell">)" << html_escape(t.fio) << "</td>";
            body << R"(<td class="num-cell">)" << html_escape(t.inn) << "</td>";
            body << R"(<td class="num-cell">)" << html_escape(t.phone) << "</td>";
            body << R"(<td class="num-cell">)" << html_escape(t.room_number) << "</td>";
            body << R"(<td class="num-cell">)" << t.area_total_sqm << "</td>";
            body << R"(<td class="num-cell">)" << t.area_trade_sqm << "</td>";
            body << R"(<td class="num-cell"><div class="actions">)";
            body << R"(<a class="small-btn doc" href="/contracts?tenant_id=)" << t.id << R"(">Договоры</a>)";
            body << R"(<a class="small-btn batch" href="/contracts/batch/new?tenant_id=)" << t.id << R"(">Пачка</a>)";
            body << R"(<a class="small-btn edit" href="/tenants/edit?id=)" << t.id << R"(">Редактировать</a>)";
            body << R"(<a class="small-btn del" href="/tenants/delete?id=)" << t.id
                 << R"html(" onclick="return confirm('Удалить арендатора?')">Удалить</a>)html";
            body << "</div></td></tr>";
        }
    }

    body << "</tbody></table></div>";
    return body.str();
}

namespace {
void field(std::ostringstream &body, const char *label, const char *name, const std::string &value,
           const char *placeholder = "", bool required = true) {
    body << R"(<div class="field"><label>)" << label << R"(</label><input name=")" << name << R"(")"
         << (required ? " required" : "") << R"( placeholder=")" << placeholder << R"(" value=")"
         << html_escape(value) << R"("></div>)";
}
}  // namespace

std::string build_tenant_form_page(const Tenant &tenant, bool is_edit, std::optional<std::string> error) {
    std::ostringstream body;
    body << R"(<h1>)" << (is_edit ? "Редактирование арендатора" : "Добавление арендатора") << "</h1>";

    if (error) {
        body << R"(<div class="banner error">)" << html_escape(*error) << "</div>";
    }

    body << R"(<form action=")" << (is_edit ? "/tenants/update" : "/tenants/add") << R"(" method="post">)";
    if (is_edit) {
        body << R"(<input type="hidden" name="id" value=")" << tenant.id << R"(">)";
    }

    body << R"(<div class="form-grid">)";
    field(body, "ФИО / наименование", "fio", tenant.fio, "Глушко Валерий Владимирович");
    field(body, "Основание деятельности", "activity_basis", tenant.activity_basis,
          "индивидуальный предприниматель, действующий на основании свидетельства 318392600011300 от 13.03.2018");
    field(body, "Адрес регистрации", "address", tenant.address, "Калининградская обл., г. Гусев, ул. Морская, д.1а, кв.81");
    body << "</div>";

    body << R"(<div class="pair-grid">)";
    field(body, "ИНН", "inn", tenant.inn, "390202674248");
    field(body, "ОГРН/ОГРНИП", "ogrn", tenant.ogrn, "318392600011300", false);
    field(body, "Паспортные данные", "passport_data", tenant.passport_data,
          "2711 371680 выдан ОУФМС России 22.12.2011 г.", false);
    field(body, "Телефон", "phone", tenant.phone, "89097786408");
    field(body, "Email", "email", tenant.email, "", false);
    field(body, "№ помещения", "room_number", tenant.room_number, "II-10-9");
    body << R"(<div class="field"><label>Площадь помещения, м²</label><input name="area_total_sqm" type="number" step="0.1" required value=")"
         << tenant.area_total_sqm << R"("></div>)";
    body << R"(<div class="field"><label>Торговая площадь, м²</label><input name="area_trade_sqm" type="number" step="0.1" required value=")"
         << tenant.area_trade_sqm << R"("></div>)";
    body << "</div>";

    body << R"(<div class="field"><label>Заметки</label><textarea name="notes">)" << html_escape(tenant.notes)
         << "</textarea></div>";

    body << R"(<div class="actions-row">
<button class="btn" type="submit">)"
         << (is_edit ? "Сохранить" : "Создать арендатора") << R"(</button>
<a class="back" href="/tenants">Отмена</a>
</div></form>)";

    return body.str();
}

}  // namespace pages
