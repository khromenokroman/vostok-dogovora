#include "format.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>

namespace fmt_util {

std::string html_escape(std::string_view text) {
    std::string out;
    out.reserve(text.size());
    for (char c : text) {
        switch (c) {
            case '&':
                out += "&amp;";
                break;
            case '<':
                out += "&lt;";
                break;
            case '>':
                out += "&gt;";
                break;
            case '"':
                out += "&quot;";
                break;
            case '\'':
                out += "&#39;";
                break;
            default:
                out.push_back(c);
        }
    }
    return out;
}

std::string format_money(long long kopecks) {
    if (kopecks < 0) kopecks = 0;
    long long rub = kopecks / 100;
    int kop = static_cast<int>(kopecks % 100);

    std::string digits = std::to_string(rub);
    std::string grouped;
    int cnt = 0;
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        if (cnt != 0 && cnt % 3 == 0) grouped.push_back(' ');
        grouped.push_back(*it);
        ++cnt;
    }
    std::reverse(grouped.begin(), grouped.end());

    char buf[8];
    std::snprintf(buf, sizeof(buf), ",%02d", kop);
    return grouped + buf;
}

std::string format_date_ru(std::string_view iso_date) {
    if (iso_date.size() != 10) return std::string(iso_date);
    return std::string(iso_date.substr(8, 2)) + "." + std::string(iso_date.substr(5, 2)) + "." +
           std::string(iso_date.substr(0, 4));
}

std::string format_date_long_ru(std::string_view iso_date) {
    if (iso_date.size() != 10) return std::string(iso_date);
    static const char *kMonths[] = {"января", "февраля", "марта",     "апреля",   "мая",      "июня",
                                     "июля",   "августа",  "сентября", "октября",  "ноября",   "декабря"};
    int day = std::stoi(std::string(iso_date.substr(8, 2)));
    int month = std::stoi(std::string(iso_date.substr(5, 2)));
    std::string year = std::string(iso_date.substr(0, 4));
    if (month < 1 || month > 12) return std::string(iso_date);
    return std::to_string(day) + " " + kMonths[month - 1] + " " + year + " г.";
}

std::chrono::year_month_day parse_iso_date(std::string_view iso_date) {
    int y = std::stoi(std::string(iso_date.substr(0, 4)));
    unsigned mo = static_cast<unsigned>(std::stoi(std::string(iso_date.substr(5, 2))));
    unsigned d = static_cast<unsigned>(std::stoi(std::string(iso_date.substr(8, 2))));
    return std::chrono::year{y} / std::chrono::month{mo} / std::chrono::day{d};
}

std::string format_iso_date(std::chrono::year_month_day date) {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%04d-%02u-%02u", static_cast<int>(date.year()),
                  static_cast<unsigned>(date.month()), static_cast<unsigned>(date.day()));
    return buf;
}

long long parse_amount_to_kopecks(std::string_view text) {
    std::string cleaned;
    for (char c : text) {
        if (std::isspace(static_cast<unsigned char>(c))) continue;
        cleaned.push_back(c == ',' ? '.' : c);
    }
    if (cleaned.empty()) return 0;

    auto dot = cleaned.find('.');
    std::string int_part = dot == std::string::npos ? cleaned : cleaned.substr(0, dot);
    std::string frac_part = dot == std::string::npos ? "" : cleaned.substr(dot + 1);
    if (frac_part.size() > 2) frac_part = frac_part.substr(0, 2);
    frac_part.resize(2, '0');

    long long rub = int_part.empty() ? 0 : std::stoll(int_part);
    long long kop = std::stoll(frac_part);
    return rub * 100 + kop;
}

std::string url_encode(std::string_view text) {
    static const char *hex = "0123456789ABCDEF";
    std::string out;
    out.reserve(text.size() * 3);
    for (unsigned char c : text) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            out.push_back(static_cast<char>(c));
        } else {
            out.push_back('%');
            out.push_back(hex[c >> 4]);
            out.push_back(hex[c & 0x0F]);
        }
    }
    return out;
}

}  // namespace fmt_util
