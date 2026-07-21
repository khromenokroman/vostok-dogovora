#include <gtest/gtest.h>

#include "business/number_to_words.hpp"

TEST(NumberToWords, ZeroRubles) { EXPECT_EQ(nw::amount_to_words_ru(0), "Ноль рублей 00 копеек"); }

TEST(NumberToWords, OneRubleSingularForm) { EXPECT_EQ(nw::amount_to_words_ru(100), "Один рубль 00 копеек"); }

TEST(NumberToWords, TwoRublesFewForm) { EXPECT_EQ(nw::amount_to_words_ru(200), "Два рубля 00 копеек"); }

TEST(NumberToWords, FiveRublesManyForm) { EXPECT_EQ(nw::amount_to_words_ru(500), "Пять рублей 00 копеек"); }

TEST(NumberToWords, ElevenToFourteenAlwaysManyForm) {
    EXPECT_EQ(nw::amount_to_words_ru(1100), "Одиннадцать рублей 00 копеек");
    EXPECT_EQ(nw::amount_to_words_ru(1400), "Четырнадцать рублей 00 копеек");
}

TEST(NumberToWords, TwentyOneUsesSingularForm) {
    EXPECT_EQ(nw::amount_to_words_ru(2100), "Двадцать один рубль 00 копеек");
}

TEST(NumberToWords, KopecksSingularFewMany) {
    EXPECT_EQ(nw::amount_to_words_ru(101), "Один рубль 01 копейка");
    EXPECT_EQ(nw::amount_to_words_ru(102), "Один рубль 02 копейки");
    EXPECT_EQ(nw::amount_to_words_ru(105), "Один рубль 05 копеек");
    EXPECT_EQ(nw::amount_to_words_ru(111), "Один рубль 11 копеек");
}

TEST(NumberToWords, SampleAmountFromContractTemplate) {
    // 8 000 руб. — сумма из образца договора («Восемь тысяч рублей 00 копеек»)
    EXPECT_EQ(nw::amount_to_words_ru(800000), "Восемь тысяч рублей 00 копеек");
}

TEST(NumberToWords, ThousandsAreFeminine) {
    EXPECT_EQ(nw::integer_to_words_ru(1000), "Одна тысяча");
    EXPECT_EQ(nw::integer_to_words_ru(2000), "Две тысячи");
    EXPECT_EQ(nw::integer_to_words_ru(5000), "Пять тысяч");
}

TEST(NumberToWords, MillionsAreMasculine) {
    EXPECT_EQ(nw::integer_to_words_ru(1000000), "Один миллион");
    EXPECT_EQ(nw::integer_to_words_ru(2000000), "Два миллиона");
    EXPECT_EQ(nw::integer_to_words_ru(5000000), "Пять миллионов");
}

TEST(NumberToWords, FeminineLastTripletFlag) {
    EXPECT_EQ(nw::integer_to_words_ru(1, true), "Одна");
    EXPECT_EQ(nw::integer_to_words_ru(2, true), "Две");
    EXPECT_EQ(nw::integer_to_words_ru(1, false), "Один");
    EXPECT_EQ(nw::integer_to_words_ru(2, false), "Два");
}

TEST(NumberToWords, ZeroInteger) { EXPECT_EQ(nw::integer_to_words_ru(0), "Ноль"); }

TEST(NumberToWords, CompoundNumber) { EXPECT_EQ(nw::integer_to_words_ru(21345), "Двадцать одна тысяча триста сорок пять"); }
