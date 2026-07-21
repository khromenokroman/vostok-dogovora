#include <gtest/gtest.h>

#include <chrono>

#include "business/contract_batch.hpp"
#include "html/format.hpp"

using contract_batch::months_breakdown_for_amount;
using contract_batch::split_rent_period_into_batch;

TEST(ContractBatch, BracketBoundariesMatch1CTable) {
    EXPECT_EQ(months_breakdown_for_amount(9090LL * 100), (std::vector<int>{11}));
    EXPECT_EQ(months_breakdown_for_amount(9091LL * 100), (std::vector<int>{5, 5, 1}));
    EXPECT_EQ(months_breakdown_for_amount(20000LL * 100), (std::vector<int>{5, 5, 1}));
    EXPECT_EQ(months_breakdown_for_amount(20001LL * 100), (std::vector<int>{4, 4, 3}));
    EXPECT_EQ(months_breakdown_for_amount(25000LL * 100), (std::vector<int>{4, 4, 3}));
    EXPECT_EQ(months_breakdown_for_amount(25001LL * 100), (std::vector<int>{3, 3, 3, 2}));
    EXPECT_EQ(months_breakdown_for_amount(33333LL * 100), (std::vector<int>{3, 3, 3, 2}));
    EXPECT_EQ(months_breakdown_for_amount(33334LL * 100), (std::vector<int>{2, 2, 2, 2, 2, 1}));
    EXPECT_EQ(months_breakdown_for_amount(50000LL * 100), (std::vector<int>{2, 2, 2, 2, 2, 1}));
    EXPECT_EQ(months_breakdown_for_amount(50001LL * 100), std::vector<int>(11, 1));
}

TEST(ContractBatch, TotalDurationIsAlwaysElevenMonths) {
    for (long long rub : {1000LL, 9090LL, 15000LL, 21000LL, 30000LL, 40000LL, 60000LL, 100000LL}) {
        auto breakdown = months_breakdown_for_amount(rub * 100);
        int sum = 0;
        for (int m : breakdown) sum += m;
        EXPECT_EQ(sum, 11) << "сумма аренды=" << rub;
    }
}

TEST(ContractBatch, SingleContractCoversElevenMonths) {
    auto start = fmt_util::parse_iso_date("2018-09-10");
    auto periods = split_rent_period_into_batch(start, 8000LL * 100);
    ASSERT_EQ(periods.size(), 1u);
    EXPECT_EQ(periods[0].months, 11);
    EXPECT_EQ(fmt_util::format_iso_date(periods[0].start), "2018-09-10");
    EXPECT_EQ(fmt_util::format_iso_date(periods[0].end), "2019-08-09");
}

TEST(ContractBatch, PeriodsAreContiguousWithoutGaps) {
    auto start = fmt_util::parse_iso_date("2018-09-10");
    auto periods = split_rent_period_into_batch(start, 21000LL * 100);
    ASSERT_EQ(periods.size(), 3u);
    EXPECT_EQ(fmt_util::format_iso_date(periods[0].start), "2018-09-10");
    EXPECT_EQ(fmt_util::format_iso_date(periods[0].end), "2019-01-09");
    EXPECT_EQ(fmt_util::format_iso_date(periods[1].start), "2019-01-10");
    EXPECT_EQ(fmt_util::format_iso_date(periods[1].end), "2019-05-09");
    EXPECT_EQ(fmt_util::format_iso_date(periods[2].start), "2019-05-10");
    EXPECT_EQ(fmt_util::format_iso_date(periods[2].end), "2019-08-09");

    for (size_t i = 1; i < periods.size(); ++i) {
        auto prev_end_next_day = std::chrono::sys_days{periods[i - 1].end} + std::chrono::days{1};
        EXPECT_EQ(prev_end_next_day, std::chrono::sys_days{periods[i].start})
            << "разрыв между договорами пачки на позиции " << i;
    }
}

TEST(ContractBatch, ElevenSingleMonthContractsForHighRent) {
    auto start = fmt_util::parse_iso_date("2018-09-10");
    auto periods = split_rent_period_into_batch(start, 60000LL * 100);
    ASSERT_EQ(periods.size(), 11u);
    for (const auto &p : periods) EXPECT_EQ(p.months, 1);
    EXPECT_EQ(fmt_util::format_iso_date(periods.back().end), "2019-08-09");
}
