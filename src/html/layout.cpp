#include "layout.hpp"

#include <sstream>

namespace layout {

const std::string_view kAppCss = R"css(
:root {
    --bg1: #1f2a44;
    --bg2: #243b55;
    --text: #f5f5f5;
    --accent2: #06d6a0;
    --danger: #ef476f;
    --shadow: 0 12px 30px rgba(0,0,0,0.28);
    --line: rgba(255,255,255,0.12);
}
* { box-sizing: border-box; }
a { color: #8ecae6; }
a:visited { color: #8ecae6; }
html, body {
    width: 100%;
    margin: 0;
    min-height: 100%;
    font-family: Cambria, serif;
    color: var(--text);
}
body {
    min-height: 100vh;
    background:
        radial-gradient(circle at top left, rgba(255,209,102,0.20), transparent 28%),
        radial-gradient(circle at bottom right, rgba(6,214,160,0.18), transparent 30%),
        linear-gradient(135deg, var(--bg1), var(--bg2));
    padding: 16px;
}
.navbar {
    max-width: 1200px;
    margin: 0 auto 14px;
    display: flex;
    gap: 8px;
    flex-wrap: wrap;
}
.navbar a {
    display: inline-flex;
    align-items: center;
    padding: 10px 18px;
    border-radius: 14px;
    text-decoration: none;
    color: #fff;
    font-weight: 700;
    font-size: 15px;
    background: rgba(255,255,255,0.08);
    border: 1px solid var(--line);
}
.navbar a.active {
    background: linear-gradient(135deg, var(--accent2), #118ab2);
}
.wrapper {
    width: 100%;
    max-width: 1200px;
    margin: 0 auto;
    background: rgba(255,255,255,0.05);
    backdrop-filter: blur(10px);
    border: 1px solid var(--line);
    border-radius: 28px;
    box-shadow: var(--shadow);
    padding: 24px;
}
.wrapper.narrow { max-width: 980px; }
.header {
    display: flex;
    justify-content: space-between;
    align-items: flex-end;
    gap: 20px;
    flex-wrap: wrap;
    margin-bottom: 18px;
}
h1 { margin: 0; font-size: 34px; }
.subtitle { margin: 8px 0 0; font-size: 17px; opacity: 0.88; }
.header-actions { display: flex; gap: 10px; flex-wrap: wrap; align-items: center; }
.header-btn, .btn, .back {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    min-height: 52px;
    padding: 0 20px;
    border-radius: 16px;
    border: none;
    cursor: pointer;
    text-decoration: none;
    font-family: Cambria, serif;
    font-size: 16px;
    font-weight: 700;
    color: #fff;
    box-shadow: 0 8px 18px rgba(0,0,0,0.18);
    white-space: nowrap;
}
.btn { background: linear-gradient(135deg, var(--accent2), #118ab2); }
.back { background: rgba(255,255,255,0.10); border: 1px solid var(--line); box-shadow: none; }
.header-btn.primary, .add-tenant { background: linear-gradient(135deg, var(--accent2), #118ab2); }
.header-btn.batch { background: linear-gradient(135deg, #ffd166, #f4a261); color: #1f2a44; }
.header-btn.print-all { background: linear-gradient(135deg, #8ecae6, #219ebc); }
.header-btn.settings { background: rgba(255,255,255,0.10); border: 1px solid var(--line); }

.filter-bar {
    display: flex;
    gap: 10px;
    flex-wrap: wrap;
    align-items: center;
    margin-bottom: 16px;
}
.filter-bar select, .filter-bar input {
    padding: 10px 12px;
    border-radius: 12px;
    border: 1px solid var(--line);
    background: rgba(255,255,255,0.10);
    color: #fff;
    font-family: Cambria, serif;
    font-size: 15px;
}

.table-wrap {
    width: 100%;
    overflow: auto;
    border-radius: 22px;
    border: 1px solid rgba(255,255,255,0.10);
}
table {
    width: 100%;
    border-collapse: collapse;
    background: rgba(255,255,255,0.05);
    font-family: Cambria, serif;
}
th, td {
    padding: 12px 10px;
    border-bottom: 1px solid rgba(255,255,255,0.08);
    font-size: 15px;
    vertical-align: middle;
    overflow-wrap: anywhere;
}
th { background: rgba(255,255,255,0.12); text-align: center; white-space: nowrap; }
td.text-cell { text-align: left; }
td.num-cell { text-align: center; white-space: nowrap; }
tr:hover { background: rgba(255,255,255,0.08); }
tr.row-unsigned { background: rgba(239,71,111,0.10); }

.actions { display: flex; gap: 6px; flex-wrap: nowrap; align-items: center; white-space: nowrap; justify-content: center; }
.small-btn {
    min-height: 36px;
    padding: 0 12px;
    border-radius: 12px;
    font-size: 13px;
    color: #fff;
    text-decoration: none;
    display: inline-flex;
    align-items: center;
    justify-content: center;
    white-space: nowrap;
    border: none;
    cursor: pointer;
    font-family: Cambria, serif;
}
.small-btn.doc { background: linear-gradient(135deg, #8ecae6, #219ebc); }
.small-btn.edit { background: linear-gradient(135deg, #ffd166, #f4a261); color: #1f2a44; }
.small-btn.del { background: linear-gradient(135deg, var(--danger), #c9184a); }
.small-btn.batch { background: linear-gradient(135deg, #b892ff, #7048e8); }

.badge {
    display: inline-flex;
    align-items: center;
    padding: 5px 12px;
    border-radius: 999px;
    font-size: 13px;
    font-weight: 700;
    color: #fff;
    white-space: nowrap;
}
.badge.signed { background: linear-gradient(135deg, var(--accent2), #118ab2); }
.badge.unsigned { background: linear-gradient(135deg, var(--danger), #c9184a); }

.empty { padding: 24px; text-align: center; opacity: 0.9; }

.banner {
    border-radius: 16px;
    padding: 14px 18px;
    margin-bottom: 16px;
    font-size: 15px;
    font-weight: 700;
}
.banner.error { background: rgba(239,71,111,0.20); border: 1px solid rgba(239,71,111,0.5); }
.banner.notice { background: rgba(6,214,160,0.18); border: 1px solid rgba(6,214,160,0.45); }

.stat-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 14px;
    margin-bottom: 20px;
}
.stat-card {
    background: rgba(255,255,255,0.07);
    border: 1px solid var(--line);
    border-radius: 18px;
    padding: 18px 20px;
}
.stat-card .value { font-size: 32px; font-weight: 700; }
.stat-card .label { font-size: 14px; opacity: 0.85; margin-top: 4px; }
.stat-card.alert .value { color: #ffd166; }

form.inline { display: inline; }

.form-grid { display: grid; grid-template-columns: 1fr; gap: 14px; margin-bottom: 18px; }
.pair-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 14px; }
.field { display: grid; gap: 8px; }
label { font-size: 15px; font-weight: 700; }
input, select, textarea {
    width: 100%;
    padding: 13px 14px;
    border-radius: 14px;
    border: 1px solid rgba(255,255,255,0.14);
    background: rgba(255,255,255,0.10);
    color: #fff;
    outline: none;
    font-family: Cambria, serif;
    font-size: 16px;
}
textarea { resize: vertical; min-height: 70px; }
select option { color: #1f2a44; }
.actions-row { display: flex; gap: 12px; flex-wrap: wrap; margin-top: 8px; }
.hint { font-size: 13px; opacity: 0.75; margin-top: 4px; }
.batch-preview { background: rgba(255,255,255,0.07); border-radius: 16px; padding: 16px; margin-top: 16px; }
.batch-preview table { background: transparent; }

@media (max-width: 900px) {
    th, td { font-size: 13px; padding: 9px 6px; }
}
)css";

const std::string_view kPrintCss = R"css(
:root {
    --bg1: #1f2a44;
    --bg2: #243b55;
    --paper: #ffffff;
    --text: #1a1a1a;
    --line: #ccc;
}
* { box-sizing: border-box; }
body {
    margin: 0;
    background: linear-gradient(135deg, var(--bg1), var(--bg2));
    font-family: Cambria, serif;
    color: var(--text);
    padding: 16px;
}
.toolbar {
    max-width: 900px;
    margin: 0 auto 16px;
    display: flex;
    gap: 10px;
}
.toolbar a {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    min-height: 44px;
    padding: 0 18px;
    border-radius: 12px;
    text-decoration: none;
    color: #fff;
    font-weight: 700;
    font-family: Cambria, serif;
    background: linear-gradient(135deg, #0b5ed7, #0a58ca);
}
.toolbar a.print { background: linear-gradient(135deg, #198754, #146c43); }

.print-page {
    max-width: 900px;
    margin: 0 auto 24px;
    background: var(--paper);
    border-radius: 10px;
    box-shadow: 0 10px 24px rgba(0,0,0,0.25);
    padding: 36px 42px;
    page-break-after: always;
}
.print-page:last-of-type { page-break-after: auto; margin-bottom: 0; }

.doc-title { text-align: center; font-weight: 700; font-size: 16px; margin-bottom: 4px; }
.doc-subtitle { text-align: center; font-size: 15px; margin-bottom: 4px; }
.doc-place-date { display: flex; justify-content: space-between; margin: 14px 0 18px; font-size: 15px; }
.doc-intro { text-align: justify; font-size: 14.5px; line-height: 1.45; margin-bottom: 14px; }
.doc-section-title { text-align: center; font-weight: 700; font-size: 15px; margin: 18px 0 8px; }
.doc-p { text-align: justify; font-size: 14px; line-height: 1.5; margin: 0 0 8px; white-space: pre-line; }
.doc-requisites { display: flex; gap: 24px; margin-top: 10px; }
.doc-requisites .col { flex: 1; font-size: 13.5px; line-height: 1.5; white-space: pre-line; }
.doc-requisites .col b { display: block; margin-bottom: 6px; }
.doc-signatures { display: flex; gap: 24px; margin-top: 32px; }
.doc-signatures .col { flex: 1; text-align: center; font-size: 14px; }
.doc-sign-line { margin-top: 46px; border-top: 1px solid #333; padding-top: 6px; }

@media print {
    body { background: #fff; padding: 0; }
    .toolbar { display: none; }
    .print-page { box-shadow: none; border-radius: 0; max-width: none; margin: 0; padding: 14mm 16mm; }
}
@page { size: A4; margin: 10mm; }
)css";

std::string nav_bar(std::string_view active_section) {
    struct Item {
        const char *href;
        const char *label;
        const char *key;
    };
    static const Item items[] = {
        {"/", "Дашборд", "dashboard"},
        {"/tenants", "Арендаторы", "tenants"},
        {"/contracts", "Договоры", "contracts"},
        {"/contracts?signed=0", "Неподписанные", "unsigned"},
        {"/settings", "Настройки", "settings"},
    };

    std::ostringstream out;
    out << R"(<div class="navbar">)";
    for (const auto &item : items) {
        out << R"(<a href=")" << item.href << R"(")";
        if (active_section == item.key) out << R"( class="active")";
        out << ">" << item.label << "</a>";
    }
    out << "</div>";
    return out.str();
}

std::string page_shell(std::string_view title, std::string_view active_section, std::string_view body_html) {
    std::ostringstream out;
    out << R"(<!doctype html>
<html lang="ru">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>)"
        << title << R"(</title>
<style>)"
        << kAppCss << R"(</style>
</head>
<body>
)"
        << nav_bar(active_section) << R"(<div class="wrapper">
)"
        << body_html << R"(
</div>
</body>
</html>)";
    return out.str();
}

}  // namespace layout
