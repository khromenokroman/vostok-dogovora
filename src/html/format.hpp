#pragma once

#include <chrono>
#include <string>
#include <string_view>

namespace fmt_util {

std::string html_escape(std::string_view text);

// 800000 копеек -> "8 000,00"
std::string format_money(long long kopecks);

// "2018-09-10" -> "10.09.2018"
std::string format_date_ru(std::string_view iso_date);

// "2018-09-10" -> "10 сентября 2018 г."
std::string format_date_long_ru(std::string_view iso_date);

std::chrono::year_month_day parse_iso_date(std::string_view iso_date);
std::string format_iso_date(std::chrono::year_month_day date);

// Разбирает пользовательский ввод суммы ("8000", "8000.50", "8000,50") в копейки.
long long parse_amount_to_kopecks(std::string_view text);

// Процентное кодирование для использования в query-строке (нужно для редиректов
// с сообщениями об ошибках на кириллице — HTTP-заголовки должны быть ASCII).
std::string url_encode(std::string_view text);

}  // namespace fmt_util
