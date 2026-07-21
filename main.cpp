#include <iostream>

#include "src/app.hpp"
#include "src/business/contract_batch.hpp"
#include "src/business/number_to_words.hpp"
#include "src/config.hpp"
#include "src/html/format.hpp"

namespace {

void run_selftest() {
    std::cout << "=== Число прописью ===" << std::endl;
    for (long long kopecks : {0LL, 100LL, 150LL, 1500LL, 100000LL, 800000LL, 2100050LL, 1000000000LL}) {
        std::cout << kopecks << " коп. -> " << nw::amount_to_words_ru(kopecks) << std::endl;
    }

    std::cout << std::endl << "=== Разбивка пачки договоров по сумме ===" << std::endl;
    for (long long rub : {9090LL, 9091LL, 20000LL, 20001LL, 25000LL, 25001LL, 33333LL, 33334LL, 50000LL, 50001LL}) {
        auto breakdown = contract_batch::months_breakdown_for_amount(rub * 100);
        int sum = 0;
        std::cout << rub << " руб.: ";
        for (int m : breakdown) {
            std::cout << m << " ";
            sum += m;
        }
        std::cout << " (итого " << sum << " мес., " << breakdown.size() << " договоров)" << std::endl;
    }

    std::cout << std::endl << "=== Даты периодов (сумма 21000, начало 2018-09-10) ===" << std::endl;
    auto start = fmt_util::parse_iso_date("2018-09-10");
    auto periods = contract_batch::split_rent_period_into_batch(start, 21000LL * 100);
    for (const auto &p : periods) {
        std::cout << fmt_util::format_iso_date(p.start) << " .. " << fmt_util::format_iso_date(p.end) << " ("
                  << p.months << " мес.)" << std::endl;
    }
}

}  // namespace

int main(int argc, char **argv) {
    if (argc > 1 && std::string(argv[1]) == "--selftest") {
        run_selftest();
        return 0;
    }

    std::string config_path = argc > 1 ? argv[1] : "configuration/cfg.json";
    Config config = load_config(config_path);

    App app(config);
    app.run();
    return 0;
}
