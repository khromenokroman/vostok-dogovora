#include "contracts_pages.hpp"

#include <sstream>

#include "business/number_to_words.hpp"
#include "html/format.hpp"

namespace pages {

using fmt_util::format_date_ru;
using fmt_util::format_iso_date;
using fmt_util::format_money;
using fmt_util::html_escape;

namespace {

std::string amount_input_value(int64_t kopecks) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%lld.%02lld", static_cast<long long>(kopecks / 100),
                  static_cast<long long>(kopecks % 100));
    return buf;
}

std::string current_query(const ContractsListFilter &filter) {
    std::string q;
    if (filter.tenant_id) q += "&tenant_id=" + std::to_string(*filter.tenant_id);
    if (filter.is_signed) q += std::string("&signed=") + (*filter.is_signed ? "1" : "0");
    return q;
}

}  // namespace

std::string build_contracts_list_page(const std::vector<ContractRow> &rows, const std::vector<Tenant> &all_tenants,
                                       const ContractsListFilter &filter, std::optional<std::string> error) {
    std::string redirect_qs = current_query(filter);
    std::string redirect_target = fmt_util::url_encode("/contracts" + (redirect_qs.empty() ? "" : "?" + redirect_qs.substr(1)));

    std::ostringstream body;
    body << R"(<div class="header"><div><h1>Договоры</h1>
<p class="subtitle">Список договоров аренды, статус подписания и печать.</p></div>
<div class="header-actions">
<a class="header-btn primary" href="/contracts/add)"
         << (filter.tenant_id ? "?tenant_id=" + std::to_string(*filter.tenant_id) : "")
         << R"(">Добавить договор</a>
</div></div>)";

    if (error) {
        body << R"(<div class="banner error">)" << html_escape(*error) << "</div>";
    }

    body << R"html(<form method="get" action="/contracts" class="filter-bar">
<select name="tenant_id" onchange="this.form.submit()"><option value="">Все арендаторы</option>)html";
    for (const auto &t : all_tenants) {
        body << R"(<option value=")" << t.id << R"(")"
             << (filter.tenant_id && *filter.tenant_id == t.id ? " selected" : "") << ">" << html_escape(t.fio)
             << "</option>";
    }
    body << R"html(</select>
<select name="signed" onchange="this.form.submit()">
<option value="">Все статусы</option>
<option value="1")html"
         << (filter.is_signed && *filter.is_signed ? " selected" : "") << R"(>Подписан</option>
<option value="0")"
         << (filter.is_signed && !*filter.is_signed ? " selected" : "") << R"(>Не подписан</option>
</select>
<noscript><button class="btn" type="submit">Применить</button></noscript>
</form>)";

    body << R"html(<form method="post" action="/print/selected" target="_blank">
<div class="header-actions" style="margin-bottom:14px;">
<button class="header-btn print-all" type="submit">Печать выбранных</button>
</div>
<div class="table-wrap"><table><thead><tr>
<th><input type="checkbox" onclick="document.querySelectorAll('.row-check').forEach(function(c){c.checked=this.checked;}, this)"></th>
<th>№</th>
<th>Арендатор</th>
<th>Период</th>
<th>Сумма, руб.</th>
<th>Статус</th>
<th>Пачка</th>
<th>Действия</th>
</tr></thead><tbody>)html";

    if (rows.empty()) {
        body << R"(<tr><td class="empty" colspan="8">Договоров пока нет.</td></tr>)";
    } else {
        for (const auto &row : rows) {
            const auto &c = row.contract;
            const auto &t = row.tenant;
            body << R"(<tr class=")" << (c.is_signed ? "" : "row-unsigned") << R"(">)";
            body << R"(<td class="num-cell"><input class="row-check" type="checkbox" name="ids" value=")" << c.id
                 << R"("></td>)";
            body << R"(<td class="num-cell">)" << c.id << "</td>";
            body << R"(<td class="text-cell"><a href="/contracts?tenant_id=)" << t.id << R"(">)" << html_escape(t.fio)
                 << R"(</a><br><span class="hint">)" << html_escape(t.phone) << "</span></td>";
            body << R"(<td class="num-cell">)" << format_date_ru(c.start_date) << " — " << format_date_ru(c.end_date)
                 << "</td>";
            body << R"(<td class="num-cell">)" << format_money(c.rent_kopecks) << "</td>";
            body << R"(<td class="num-cell"><a class="badge )" << (c.is_signed ? "signed" : "unsigned")
                 << R"(" href="/contracts/toggle-signed?id=)" << c.id << "&redirect=" << redirect_target << R"(">)"
                 << (c.is_signed ? "Подписан" : "Не подписан") << "</a></td>";
            body << R"(<td class="num-cell">)";
            if (c.batch_id) {
                body << R"(<a class="small-btn batch" href="/print/batch?batch_id=)" << *c.batch_id
                     << R"(" target="_blank">Пачка)"
                     << (c.batch_seq ? " #" + std::to_string(*c.batch_seq) : "") << "</a>";
            } else {
                body << R"(<span class="hint">—</span>)";
            }
            body << "</td>";
            body << R"(<td class="num-cell"><div class="actions">)";
            body << R"(<a class="small-btn doc" href="/print/contract?id=)" << c.id
                 << R"(" target="_blank">Печать</a>)";
            body << R"(<a class="small-btn edit" href="/contracts/edit?id=)" << c.id << R"(">Редактировать</a>)";
            body << R"(<a class="small-btn del" href="/contracts/delete?id=)" << c.id
                 << R"html(" onclick="return confirm('Удалить договор?')">Удалить</a>)html";
            body << "</div></td></tr>";
        }
    }

    body << "</tbody></table></div></form>";
    return body.str();
}

std::string build_contract_form_page(const Contract &contract, const std::vector<Tenant> &all_tenants, bool is_edit,
                                      std::optional<std::string> error) {
    std::ostringstream body;
    body << R"(<h1>)" << (is_edit ? "Редактирование договора" : "Добавление договора") << "</h1>";

    if (error) {
        body << R"(<div class="banner error">)" << html_escape(*error) << "</div>";
    }

    body << R"(<form action=")" << (is_edit ? "/contracts/update" : "/contracts/add") << R"(" method="post">)";
    if (is_edit) {
        body << R"(<input type="hidden" name="id" value=")" << contract.id << R"(">)";
    }

    body << R"(<div class="pair-grid">
<div class="field"><label>Арендатор</label><select name="tenant_id" required>)";
    if (!is_edit) {
        body << R"(<option value="">— выберите —</option>)";
    }
    for (const auto &t : all_tenants) {
        body << R"(<option value=")" << t.id << R"(")" << (contract.tenant_id == t.id ? " selected" : "") << ">"
             << html_escape(t.fio) << " (" << html_escape(t.room_number) << ")</option>";
    }
    body << R"(</select></div>
<div class="field"><label>Сумма аренды в месяц, руб.</label><input name="rent_amount" required value=")"
         << (is_edit ? amount_input_value(contract.rent_kopecks) : "") << R"(" placeholder="8000.00"></div>
<div class="field"><label>Дата начала</label><input type="date" name="start_date" required value=")"
         << contract.start_date << R"("></div>
<div class="field"><label>Дата окончания</label><input type="date" name="end_date" required value=")"
         << contract.end_date << R"("></div>
</div>)";

    if (is_edit) {
        body << R"(<div class="field"><label><input type="checkbox" name="is_signed" style="width:auto;" )"
             << (contract.is_signed ? "checked" : "") << R"("> Договор подписан</label></div>)";
    }

    body << R"(<div class="actions-row">
<button class="btn" type="submit">)"
         << (is_edit ? "Сохранить" : "Создать договор") << R"(</button>
<a class="back" href="/contracts">Отмена</a>
</div></form>)";

    return body.str();
}

std::string build_batch_form_page(const Tenant &tenant, const std::string &start_date, const std::string &rent_amount,
                                   const std::vector<contract_batch::BatchPeriod> &periods,
                                   std::optional<std::string> error) {
    std::ostringstream body;
    body << R"(<h1>Пачка договоров — )" << html_escape(tenant.fio) << "</h1>"
         << R"(<p class="subtitle">Помещение )" << html_escape(tenant.room_number) << ", площадь "
         << tenant.area_total_sqm << R"( м² (торговая — )" << tenant.area_trade_sqm << R"( м²).</p>)";

    if (error) {
        body << R"(<div class="banner error">)" << html_escape(*error) << "</div>";
    }

    body << R"(<form method="get" action="/contracts/batch/new">
<input type="hidden" name="tenant_id" value=")"
         << tenant.id << R"(">
<div class="pair-grid">
<div class="field"><label>Дата начала периода</label><input type="date" name="start_date" required value=")"
         << html_escape(start_date) << R"("></div>
<div class="field"><label>Сумма аренды в месяц, руб.</label><input name="rent_amount" required value=")"
         << html_escape(rent_amount) << R"(" placeholder="8000.00"></div>
</div>
<div class="actions-row">
<button class="btn" type="submit">Рассчитать разбивку</button>
<a class="back" href="/tenants">Отмена</a>
</div>
</form>)";

    if (!periods.empty()) {
        long long rent_kopecks = fmt_util::parse_amount_to_kopecks(rent_amount);
        body << R"(<div class="batch-preview"><h3 style="margin-top:0;">Будет создано договоров: )" << periods.size()
             << R"(</h3><p class="hint">Сумма каждого договора: )" << format_money(rent_kopecks) << " руб. ("
             << html_escape(nw::amount_to_words_ru(rent_kopecks)) << ")</p>";
        body << R"(<table><thead><tr><th>№</th><th>Начало</th><th>Окончание</th><th>Срок, мес.</th></tr></thead><tbody>)";
        int i = 1;
        for (const auto &p : periods) {
            body << "<tr><td class=\"num-cell\">" << i++ << "</td><td class=\"num-cell\">"
                 << format_date_ru(format_iso_date(p.start)) << "</td><td class=\"num-cell\">"
                 << format_date_ru(format_iso_date(p.end)) << "</td><td class=\"num-cell\">" << p.months
                 << "</td></tr>";
        }
        body << R"(</tbody></table>
<form method="post" action="/contracts/batch/create">
<input type="hidden" name="tenant_id" value=")"
             << tenant.id << R"(">
<input type="hidden" name="start_date" value=")" << html_escape(start_date) << R"(">
<input type="hidden" name="rent_amount" value=")" << html_escape(rent_amount) << R"(">
<div class="actions-row"><button class="btn" type="submit">Подтвердить и создать )"
             << periods.size() << R"( договор(ов)</button></div>
</form></div>)";
    }

    return body.str();
}

}  // namespace pages
