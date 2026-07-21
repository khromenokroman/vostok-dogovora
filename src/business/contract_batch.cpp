#include "contract_batch.hpp"

namespace contract_batch {

std::vector<int> months_breakdown_for_amount(long long monthly_rent_kopecks) {
    if (monthly_rent_kopecks <= 9090LL * 100) return {11};
    if (monthly_rent_kopecks <= 20000LL * 100) return {5, 5, 1};
    if (monthly_rent_kopecks <= 25000LL * 100) return {4, 4, 3};
    if (monthly_rent_kopecks <= 33333LL * 100) return {3, 3, 3, 2};
    if (monthly_rent_kopecks <= 50000LL * 100) return {2, 2, 2, 2, 2, 1};
    return std::vector<int>(11, 1);
}

namespace {

using std::chrono::days;
using std::chrono::months;
using std::chrono::sys_days;
using std::chrono::year_month_day;
using std::chrono::year_month_day_last;

// Добавляет N месяцев к дате; если получившийся день не существует в целевом
// месяце (например, 31 января + 1 месяц), берётся последний день этого месяца.
year_month_day add_months_clamped(const year_month_day &date, int n) {
    year_month_day result = date + months{n};
    if (!result.ok()) {
        result = year_month_day{year_month_day_last{result.year(), std::chrono::month_day_last{result.month()}}};
    }
    return result;
}

}  // namespace

std::vector<BatchPeriod> split_rent_period_into_batch(std::chrono::year_month_day start_date,
                                                        long long monthly_rent_kopecks) {
    std::vector<int> breakdown = months_breakdown_for_amount(monthly_rent_kopecks);

    std::vector<BatchPeriod> result;
    result.reserve(breakdown.size());

    year_month_day cursor = start_date;
    for (int n : breakdown) {
        year_month_day next_start = add_months_clamped(cursor, n);
        year_month_day end = year_month_day{sys_days{next_start} - days{1}};

        result.push_back(BatchPeriod{cursor, end, n});
        cursor = next_start;
    }

    return result;
}

}  // namespace contract_batch
