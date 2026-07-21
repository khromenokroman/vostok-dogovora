#include <gtest/gtest.h>

#include "html/format.hpp"

TEST(Format, HtmlEscapeCoversAllSpecialChars) {
    EXPECT_EQ(fmt_util::html_escape("<b>Tom & Jerry's \"cartoon\"</b>"),
              "&lt;b&gt;Tom &amp; Jerry&#39;s &quot;cartoon&quot;&lt;/b&gt;");
}

TEST(Format, HtmlEscapePassesThroughPlainText) { EXPECT_EQ(fmt_util::html_escape("Глушко В.В."), "Глушко В.В."); }

TEST(Format, FormatMoneyGroupsThousands) {
    EXPECT_EQ(fmt_util::format_money(800000), "8 000,00");
    EXPECT_EQ(fmt_util::format_money(150), "1,50");
    EXPECT_EQ(fmt_util::format_money(0), "0,00");
    EXPECT_EQ(fmt_util::format_money(123456789), "1 234 567,89");
}

TEST(Format, FormatDateRu) { EXPECT_EQ(fmt_util::format_date_ru("2018-09-10"), "10.09.2018"); }

TEST(Format, FormatDateLongRu) { EXPECT_EQ(fmt_util::format_date_long_ru("2018-09-10"), "10 сентября 2018 г."); }

TEST(Format, IsoDateRoundtrip) {
    auto d = fmt_util::parse_iso_date("2020-01-15");
    EXPECT_EQ(fmt_util::format_iso_date(d), "2020-01-15");
}

TEST(Format, ParseAmountToKopecksVariousInputs) {
    EXPECT_EQ(fmt_util::parse_amount_to_kopecks("8000"), 800000);
    EXPECT_EQ(fmt_util::parse_amount_to_kopecks("8000.5"), 800050);
    EXPECT_EQ(fmt_util::parse_amount_to_kopecks("8000,50"), 800050);
    EXPECT_EQ(fmt_util::parse_amount_to_kopecks("8 000,50"), 800050);
    EXPECT_EQ(fmt_util::parse_amount_to_kopecks(""), 0);
}

TEST(Format, UrlEncodeKeepsSafeCharsAndEncodesRest) {
    EXPECT_EQ(fmt_util::url_encode("a b"), "a%20b");
    EXPECT_EQ(fmt_util::url_encode("A-Z_0-9.~"), "A-Z_0-9.~");
    EXPECT_EQ(fmt_util::url_encode("У"), "%D0%A3");
}
