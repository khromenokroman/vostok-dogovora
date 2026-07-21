#pragma once

#include <string>
#include <string_view>

namespace layout {

// Тёмная glassmorphism-тема приложения (списки, формы, дашборд).
extern const std::string_view kAppCss;

// Светлая тема для печатных страниц (@media print, разрывы страниц).
extern const std::string_view kPrintCss;

// Оборачивает body_html в полный HTML-документ с общим <style> и навигацией.
// active_section — один из: "dashboard", "tenants", "contracts", "unsigned", "settings".
std::string page_shell(std::string_view title, std::string_view active_section, std::string_view body_html);

std::string nav_bar(std::string_view active_section);

}  // namespace layout
