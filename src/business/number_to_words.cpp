#include "number_to_words.hpp"

#include <array>
#include <cstdio>
#include <vector>

namespace nw {
namespace {

const std::array<std::string, 20> kUnitsMasc = {
    "",       "один",      "два",       "три",       "четыре",     "пять",
    "шесть",  "семь",      "восемь",    "девять",    "десять",     "одиннадцать",
    "двенадцать", "тринадцать", "четырнадцать", "пятнадцать", "шестнадцать",
    "семнадцать", "восемнадцать", "девятнадцать"};

const std::array<std::string, 20> kUnitsFem = {
    "",       "одна",      "две",       "три",       "четыре",     "пять",
    "шесть",  "семь",      "восемь",    "девять",    "десять",     "одиннадцать",
    "двенадцать", "тринадцать", "четырнадцать", "пятнадцать", "шестнадцать",
    "семнадцать", "восемнадцать", "девятнадцать"};

const std::array<std::string, 10> kTens = {"", "", "двадцать",  "тридцать", "сорок",
                                            "пятьдесят", "шестьдесят", "семьдесят",
                                            "восемьдесят", "девяносто"};

const std::array<std::string, 10> kHundreds = {"",     "сто",     "двести",   "триста",
                                                "четыреста", "пятьсот", "шестьсот",
                                                "семьсот", "восемьсот", "девятьсот"};

const std::array<std::string, 3> kThousandForms = {"тысяча", "тысячи", "тысяч"};
const std::array<std::string, 3> kMillionForms = {"миллион", "миллиона", "миллионов"};
const std::array<std::string, 3> kBillionForms = {"миллиард", "миллиарда", "миллиардов"};
const std::array<std::string, 3> kRubleForms = {"рубль", "рубля", "рублей"};
const std::array<std::string, 3> kKopeckForms = {"копейка", "копейки", "копеек"};

int plural_index(unsigned long long n) {
    unsigned long long last_two = n % 100;
    if (last_two >= 11 && last_two <= 14) {
        return 2;
    }
    unsigned long long last_one = n % 10;
    if (last_one == 1) return 0;
    if (last_one >= 2 && last_one <= 4) return 1;
    return 2;
}

const std::string &plural_word(unsigned long long n, const std::array<std::string, 3> &forms) {
    return forms[plural_index(n)];
}

void append(std::string &out, const std::string &word) {
    if (word.empty()) return;
    if (!out.empty()) out += ' ';
    out += word;
}

// n в диапазоне [0, 999]
std::string group_words(unsigned n, bool feminine) {
    std::string out;
    unsigned h = n / 100;
    unsigned rem = n % 100;
    if (h) {
        append(out, kHundreds[h]);
    }
    if (rem >= 10 && rem <= 19) {
        append(out, feminine ? kUnitsFem[rem] : kUnitsMasc[rem]);
    } else {
        unsigned t = rem / 10;
        unsigned u = rem % 10;
        if (t) append(out, kTens[t]);
        if (u) append(out, feminine ? kUnitsFem[u] : kUnitsMasc[u]);
    }
    return out;
}

std::string capitalize_first_utf8(std::string s) {
    if (s.size() >= 2) {
        auto b0 = static_cast<unsigned char>(s[0]);
        auto b1 = static_cast<unsigned char>(s[1]);
        if (b0 == 0xD0 && b1 >= 0xB0 && b1 <= 0xBF) {
            s[1] = static_cast<char>(b1 - 0x20);
            return s;
        }
        if (b0 == 0xD1 && b1 >= 0x80 && b1 <= 0x8F) {
            s[0] = static_cast<char>(0xD0);
            s[1] = static_cast<char>(b1 + 0x20);
            return s;
        }
    }
    if (!s.empty()) {
        s[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
    }
    return s;
}

}  // namespace

std::string integer_to_words_ru(unsigned long long value, bool feminine_last_triplet) {
    if (value == 0) {
        return "Ноль";
    }

    // Группы по 1000, от младших к старшим: [0]=единицы, [1]=тысячи, [2]=миллионы, [3]=миллиарды
    std::vector<unsigned> groups;
    unsigned long long v = value;
    while (v > 0) {
        groups.push_back(static_cast<unsigned>(v % 1000));
        v /= 1000;
    }

    std::string out;
    for (int i = static_cast<int>(groups.size()) - 1; i >= 0; --i) {
        unsigned g = groups[static_cast<size_t>(i)];
        if (g == 0) continue;

        bool feminine = (i == 1) || (i == 0 && feminine_last_triplet);
        append(out, group_words(g, feminine));

        if (i == 1) {
            append(out, plural_word(g, kThousandForms));
        } else if (i == 2) {
            append(out, plural_word(g, kMillionForms));
        } else if (i == 3) {
            append(out, plural_word(g, kBillionForms));
        }
    }

    return capitalize_first_utf8(out);
}

std::string amount_to_words_ru(long long total_kopecks) {
    if (total_kopecks < 0) total_kopecks = 0;
    auto rubles = static_cast<unsigned long long>(total_kopecks / 100);
    auto kopecks = static_cast<unsigned>(total_kopecks % 100);

    std::string out = integer_to_words_ru(rubles, false);
    out += ' ';
    out += plural_word(rubles, kRubleForms);

    char buf[16];
    std::snprintf(buf, sizeof(buf), "%02u", kopecks);
    out += ' ';
    out += buf;
    out += ' ';
    out += plural_word(kopecks, kKopeckForms);

    return out;
}

}  // namespace nw
