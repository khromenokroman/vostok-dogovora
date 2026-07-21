#pragma once

#include <string>

namespace nw {

// Целое число прописью, с заглавной первой буквы. feminine_last_triplet управляет
// родом последней (единичной) триады — используется для сумм в валюте, где сама
// сущность женского рода (например, копейки: "одна", "две").
std::string integer_to_words_ru(unsigned long long value, bool feminine_last_triplet = false);

// Сумма в копейках -> "Пятнадцать тысяч рублей 50 копеек".
std::string amount_to_words_ru(long long total_kopecks);

}  // namespace nw
