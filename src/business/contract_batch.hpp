#pragma once

#include <chrono>
#include <vector>

namespace contract_batch {

struct BatchPeriod {
    std::chrono::year_month_day start;
    std::chrono::year_month_day end;
    int months = 0;
};

// Таблица бракетов из 1С: сумма аренды/мес (в копейках) -> список длительностей
// договоров в месяцах. Сумма длительностей всегда равна 11 месяцам.
std::vector<int> months_breakdown_for_amount(long long monthly_rent_kopecks);

// Строит N периодов подряд (встык, без разрывов), начиная с start_date.
std::vector<BatchPeriod> split_rent_period_into_batch(std::chrono::year_month_day start_date,
                                                        long long monthly_rent_kopecks);

}  // namespace contract_batch
