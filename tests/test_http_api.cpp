// Интеграционные тесты: поднимают реальный HTTP-сервер (App) в фоновом потоке
// и дёргают его через curl (как это делал бы браузер), а состояние базы
// проверяют напрямую через репозитории на отдельном соединении к тому же
// файлу SQLite.

#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>
#include <sstream>
#include <thread>

#include "app.hpp"
#include "config.hpp"
#include "db/contract_repository.hpp"
#include "db/database.hpp"
#include "db/settings_repository.hpp"
#include "db/tenant_repository.hpp"

namespace {

constexpr int kTestPort = 18173;
const std::string kBase = "http://127.0.0.1:18173";

std::string test_db_path() {
    return (std::filesystem::temp_directory_path() / "vostok_dogovora_test_http_api.sqlite3").string();
}

struct CurlResult {
    long status = 0;
    std::string location;
    std::string body;
};

// Выполняет произвольную curl-команду (аргументы уже полностью сформированы)
// и разбирает код ответа, заголовок Location и тело.
CurlResult run_curl(const std::string &args) {
    std::string cmd = "curl -s -i " + args;
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        ADD_FAILURE() << "popen не сработал для: " << cmd;
        return {};
    }
    std::string output;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), pipe)) > 0) {
        output.append(buf, n);
    }
    pclose(pipe);

    CurlResult result;
    size_t sep = output.find("\r\n\r\n");
    std::string sep_str = "\r\n\r\n";
    if (sep == std::string::npos) {
        sep = output.find("\n\n");
        sep_str = "\n\n";
    }
    std::string headers = (sep == std::string::npos) ? output : output.substr(0, sep);
    result.body = (sep == std::string::npos) ? "" : output.substr(sep + sep_str.size());

    std::istringstream hstream(headers);
    std::string line;
    if (std::getline(hstream, line)) {
        auto pos1 = line.find(' ');
        if (pos1 != std::string::npos) {
            auto pos2 = line.find(' ', pos1 + 1);
            std::string code = line.substr(pos1 + 1, pos2 - pos1 - 1);
            try {
                result.status = std::stol(code);
            } catch (...) {
            }
        }
    }
    while (std::getline(hstream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.rfind("Location:", 0) == 0) {
            result.location = line.substr(9);
            while (!result.location.empty() && result.location.front() == ' ') result.location.erase(result.location.begin());
        }
    }
    return result;
}

CurlResult http_get(const std::string &path) { return run_curl("'" + kBase + path + "'"); }

CurlResult http_post(const std::string &path, const std::vector<std::pair<std::string, std::string>> &fields) {
    std::string args = "-X POST '" + kBase + path + "'";
    for (const auto &[k, v] : fields) {
        args += " --data-urlencode '" + k + "=" + v + "'";
    }
    return run_curl(args);
}

}  // namespace

class HttpApiTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        std::filesystem::remove(test_db_path());

        Config cfg;
        cfg.host = "127.0.0.1";
        cfg.port = kTestPort;
        cfg.db_path = test_db_path();

        s_app = std::make_unique<App>(cfg);
        s_thread = std::thread([] { s_app->run(); });

        // ждём готовности сервера
        for (int i = 0; i < 100; ++i) {
            auto r = http_get("/");
            if (r.status == 200) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        s_db = std::make_unique<Database>(test_db_path());
        s_tenants = std::make_unique<TenantRepository>(s_db->handle());
        s_contracts = std::make_unique<ContractRepository>(s_db->handle());
        s_settings = std::make_unique<SettingsRepository>(s_db->handle());
    }

    static void TearDownTestSuite() {
        s_app->stop();
        s_thread.join();
        s_settings.reset();
        s_contracts.reset();
        s_tenants.reset();
        s_db.reset();
        s_app.reset();
        std::filesystem::remove(test_db_path());
    }

    static std::optional<Tenant> find_tenant_by_fio(const std::string &fio) {
        for (auto &t : s_tenants->list()) {
            if (t.fio == fio) return t;
        }
        return std::nullopt;
    }

    static std::unique_ptr<App> s_app;
    static std::thread s_thread;
    static std::unique_ptr<Database> s_db;
    static std::unique_ptr<TenantRepository> s_tenants;
    static std::unique_ptr<ContractRepository> s_contracts;
    static std::unique_ptr<SettingsRepository> s_settings;
};

std::unique_ptr<App> HttpApiTest::s_app;
std::thread HttpApiTest::s_thread;
std::unique_ptr<Database> HttpApiTest::s_db;
std::unique_ptr<TenantRepository> HttpApiTest::s_tenants;
std::unique_ptr<ContractRepository> HttpApiTest::s_contracts;
std::unique_ptr<SettingsRepository> HttpApiTest::s_settings;

TEST_F(HttpApiTest, DashboardRespondsOk) {
    auto r = http_get("/");
    EXPECT_EQ(r.status, 200);
    EXPECT_NE(r.body.find("Восток"), std::string::npos);
}

TEST_F(HttpApiTest, AddTenantPersistsToDatabase) {
    auto r = http_post("/tenants/add",
                        {{"fio", "Иванов Иван Иванович"},
                         {"activity_basis", "ИП"},
                         {"address", "г. Гусев"},
                         {"inn", "111111111111"},
                         {"ogrn", ""},
                         {"passport_data", ""},
                         {"phone", "89990001122"},
                         {"email", ""},
                         {"room_number", "A-1"},
                         {"area_total_sqm", "12"},
                         {"area_trade_sqm", "5"},
                         {"notes", ""}});
    EXPECT_EQ(r.status, 302);

    auto tenant = find_tenant_by_fio("Иванов Иван Иванович");
    ASSERT_TRUE(tenant.has_value());
    EXPECT_EQ(tenant->inn, "111111111111");
    EXPECT_EQ(tenant->phone, "89990001122");
    EXPECT_DOUBLE_EQ(tenant->area_total_sqm, 12.0);
}

TEST_F(HttpApiTest, RenameTenantUpdatesFio) {
    http_post("/tenants/add", {{"fio", "До Переименования"},
                                {"activity_basis", ""},
                                {"address", ""},
                                {"inn", "222"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", ""},
                                {"area_total_sqm", "1"},
                                {"area_trade_sqm", "1"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("До Переименования");
    ASSERT_TRUE(tenant.has_value());

    auto r = http_post("/tenants/update", {{"id", std::to_string(tenant->id)},
                                            {"fio", "После Переименования"},
                                            {"activity_basis", ""},
                                            {"address", ""},
                                            {"inn", "222"},
                                            {"ogrn", ""},
                                            {"passport_data", ""},
                                            {"phone", ""},
                                            {"email", ""},
                                            {"room_number", ""},
                                            {"area_total_sqm", "1"},
                                            {"area_trade_sqm", "1"},
                                            {"notes", ""}});
    EXPECT_EQ(r.status, 302);

    EXPECT_FALSE(find_tenant_by_fio("До Переименования").has_value());
    auto renamed = find_tenant_by_fio("После Переименования");
    ASSERT_TRUE(renamed.has_value());
    EXPECT_EQ(renamed->id, tenant->id);
}

TEST_F(HttpApiTest, DeleteTenantWithoutContractsSucceeds) {
    http_post("/tenants/add", {{"fio", "На Удаление"},
                                {"activity_basis", ""},
                                {"address", ""},
                                {"inn", "333"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", ""},
                                {"area_total_sqm", "1"},
                                {"area_trade_sqm", "1"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("На Удаление");
    ASSERT_TRUE(tenant.has_value());

    auto r = http_get("/tenants/delete?id=" + std::to_string(tenant->id));
    EXPECT_EQ(r.status, 302);
    EXPECT_EQ(r.location, "/tenants");
    EXPECT_FALSE(find_tenant_by_fio("На Удаление").has_value());
}

TEST_F(HttpApiTest, DeleteTenantWithContractsIsBlocked) {
    http_post("/tenants/add", {{"fio", "Арендатор С Договором"},
                                {"activity_basis", ""},
                                {"address", ""},
                                {"inn", "444"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", ""},
                                {"area_total_sqm", "1"},
                                {"area_trade_sqm", "1"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("Арендатор С Договором");
    ASSERT_TRUE(tenant.has_value());

    http_post("/contracts/add", {{"tenant_id", std::to_string(tenant->id)},
                                  {"rent_amount", "5000.00"},
                                  {"start_date", "2024-01-01"},
                                  {"end_date", "2024-12-01"}});

    ContractFilter filter;
    filter.tenant_id = tenant->id;
    ASSERT_EQ(s_contracts->list(filter).size(), 1u);

    auto r = http_get("/tenants/delete?id=" + std::to_string(tenant->id));
    EXPECT_EQ(r.status, 302);
    EXPECT_NE(r.location.find("error="), std::string::npos);

    // и арендатор, и договор должны остаться нетронутыми
    EXPECT_TRUE(find_tenant_by_fio("Арендатор С Договором").has_value());
    EXPECT_EQ(s_contracts->list(filter).size(), 1u);
}

TEST_F(HttpApiTest, AddContractPersistsToDatabase) {
    http_post("/tenants/add", {{"fio", "Арендатор Для Договора"},
                                {"activity_basis", ""},
                                {"address", ""},
                                {"inn", "555"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", ""},
                                {"area_total_sqm", "1"},
                                {"area_trade_sqm", "1"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("Арендатор Для Договора");
    ASSERT_TRUE(tenant.has_value());

    auto r = http_post("/contracts/add", {{"tenant_id", std::to_string(tenant->id)},
                                           {"rent_amount", "8000.00"},
                                           {"start_date", "2018-09-10"},
                                           {"end_date", "2019-08-09"}});
    EXPECT_EQ(r.status, 302);

    ContractFilter filter;
    filter.tenant_id = tenant->id;
    auto contracts = s_contracts->list(filter);
    ASSERT_EQ(contracts.size(), 1u);
    EXPECT_EQ(contracts[0].rent_kopecks, 800000);
    EXPECT_EQ(contracts[0].start_date, "2018-09-10");
    EXPECT_EQ(contracts[0].end_date, "2019-08-09");
    EXPECT_FALSE(contracts[0].is_signed);
}

TEST_F(HttpApiTest, UpdateContractChangesFieldsAndSignedFlag) {
    http_post("/tenants/add", {{"fio", "Арендатор Для Правки"},
                                {"activity_basis", ""},
                                {"address", ""},
                                {"inn", "666"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", ""},
                                {"area_total_sqm", "1"},
                                {"area_trade_sqm", "1"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("Арендатор Для Правки");
    ASSERT_TRUE(tenant.has_value());

    http_post("/contracts/add", {{"tenant_id", std::to_string(tenant->id)},
                                  {"rent_amount", "1000.00"},
                                  {"start_date", "2024-01-01"},
                                  {"end_date", "2024-02-01"}});
    ContractFilter filter;
    filter.tenant_id = tenant->id;
    auto before = s_contracts->list(filter);
    ASSERT_EQ(before.size(), 1u);
    int64_t contract_id = before[0].id;

    auto r = http_post("/contracts/update", {{"id", std::to_string(contract_id)},
                                              {"tenant_id", std::to_string(tenant->id)},
                                              {"rent_amount", "9999.00"},
                                              {"start_date", "2024-03-01"},
                                              {"end_date", "2024-04-01"},
                                              {"is_signed", "on"}});
    EXPECT_EQ(r.status, 302);

    auto updated = s_contracts->get(contract_id);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->rent_kopecks, 999900);
    EXPECT_EQ(updated->start_date, "2024-03-01");
    EXPECT_TRUE(updated->is_signed);
}

TEST_F(HttpApiTest, ToggleSignedFlipsBackAndForth) {
    http_post("/tenants/add", {{"fio", "Арендатор Для Подписи"},
                                {"activity_basis", ""},
                                {"address", ""},
                                {"inn", "777"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", ""},
                                {"area_total_sqm", "1"},
                                {"area_trade_sqm", "1"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("Арендатор Для Подписи");
    ASSERT_TRUE(tenant.has_value());

    http_post("/contracts/add", {{"tenant_id", std::to_string(tenant->id)},
                                  {"rent_amount", "1000.00"},
                                  {"start_date", "2024-01-01"},
                                  {"end_date", "2024-02-01"}});
    ContractFilter filter;
    filter.tenant_id = tenant->id;
    auto contracts = s_contracts->list(filter);
    ASSERT_EQ(contracts.size(), 1u);
    int64_t contract_id = contracts[0].id;
    EXPECT_FALSE(s_contracts->get(contract_id)->is_signed);

    http_get("/contracts/toggle-signed?id=" + std::to_string(contract_id));
    EXPECT_TRUE(s_contracts->get(contract_id)->is_signed);

    http_get("/contracts/toggle-signed?id=" + std::to_string(contract_id));
    EXPECT_FALSE(s_contracts->get(contract_id)->is_signed);
}

TEST_F(HttpApiTest, DeleteContractRemovesRow) {
    http_post("/tenants/add", {{"fio", "Арендатор Для Удаления Договора"},
                                {"activity_basis", ""},
                                {"address", ""},
                                {"inn", "888"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", ""},
                                {"area_total_sqm", "1"},
                                {"area_trade_sqm", "1"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("Арендатор Для Удаления Договора");
    ASSERT_TRUE(tenant.has_value());

    http_post("/contracts/add", {{"tenant_id", std::to_string(tenant->id)},
                                  {"rent_amount", "1000.00"},
                                  {"start_date", "2024-01-01"},
                                  {"end_date", "2024-02-01"}});
    ContractFilter filter;
    filter.tenant_id = tenant->id;
    auto contracts = s_contracts->list(filter);
    ASSERT_EQ(contracts.size(), 1u);
    int64_t contract_id = contracts[0].id;

    auto r = http_get("/contracts/delete?id=" + std::to_string(contract_id));
    EXPECT_EQ(r.status, 302);
    EXPECT_FALSE(s_contracts->get(contract_id).has_value());

    // теперь арендатора без договоров можно удалить
    auto del = http_get("/tenants/delete?id=" + std::to_string(tenant->id));
    EXPECT_EQ(del.status, 302);
    EXPECT_EQ(del.location, "/tenants");
}

TEST_F(HttpApiTest, CreateBatchSplitsAccordingToBrackets) {
    http_post("/tenants/add", {{"fio", "Арендатор Для Пачки"},
                                {"activity_basis", ""},
                                {"address", ""},
                                {"inn", "999"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", ""},
                                {"area_total_sqm", "1"},
                                {"area_trade_sqm", "1"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("Арендатор Для Пачки");
    ASSERT_TRUE(tenant.has_value());

    auto r = http_post("/contracts/batch/create", {{"tenant_id", std::to_string(tenant->id)},
                                                     {"start_date", "2020-01-15"},
                                                     {"rent_amount", "21000"}});
    EXPECT_EQ(r.status, 302);

    ContractFilter filter;
    filter.tenant_id = tenant->id;
    auto contracts = s_contracts->list(filter);
    ASSERT_EQ(contracts.size(), 3u);  // бракет 20000<x<=25000 -> {4,4,3}

    // все договоры пачки должны ссылаться на один и тот же batch_id
    ASSERT_TRUE(contracts[0].batch_id.has_value());
    int64_t batch_id = *contracts[0].batch_id;
    for (const auto &c : contracts) {
        ASSERT_TRUE(c.batch_id.has_value());
        EXPECT_EQ(*c.batch_id, batch_id);
        EXPECT_EQ(c.rent_kopecks, 2100000);
    }

    auto by_batch = s_contracts->list_by_batch(batch_id);
    ASSERT_EQ(by_batch.size(), 3u);
    EXPECT_EQ(by_batch[0].batch_seq, 1);
    EXPECT_EQ(by_batch[1].batch_seq, 2);
    EXPECT_EQ(by_batch[2].batch_seq, 3);
}

TEST_F(HttpApiTest, CreateBatchSingleBracketHasNoBatchId) {
    http_post("/tenants/add", {{"fio", "Арендатор Одного Договора"},
                                {"activity_basis", ""},
                                {"address", ""},
                                {"inn", "1010"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", ""},
                                {"area_total_sqm", "1"},
                                {"area_trade_sqm", "1"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("Арендатор Одного Договора");
    ASSERT_TRUE(tenant.has_value());

    http_post("/contracts/batch/create",
              {{"tenant_id", std::to_string(tenant->id)}, {"start_date", "2018-09-10"}, {"rent_amount", "8000"}});

    ContractFilter filter;
    filter.tenant_id = tenant->id;
    auto contracts = s_contracts->list(filter);
    ASSERT_EQ(contracts.size(), 1u);
    EXPECT_FALSE(contracts[0].batch_id.has_value());
}

TEST_F(HttpApiTest, PrintSingleContractContainsExpectedText) {
    http_post("/tenants/add", {{"fio", "Печатный Арендатор"},
                                {"activity_basis", "индивидуальный предприниматель"},
                                {"address", ""},
                                {"inn", "1111"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", "II-10-9"},
                                {"area_total_sqm", "16"},
                                {"area_trade_sqm", "6"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("Печатный Арендатор");
    ASSERT_TRUE(tenant.has_value());

    http_post("/contracts/add", {{"tenant_id", std::to_string(tenant->id)},
                                  {"rent_amount", "8000.00"},
                                  {"start_date", "2018-09-10"},
                                  {"end_date", "2019-08-09"}});
    ContractFilter filter;
    filter.tenant_id = tenant->id;
    auto contracts = s_contracts->list(filter);
    ASSERT_EQ(contracts.size(), 1u);

    auto r = http_get("/print/contract?id=" + std::to_string(contracts[0].id));
    EXPECT_EQ(r.status, 200);
    EXPECT_NE(r.body.find("Восемь тысяч рублей 00 копеек"), std::string::npos);
    EXPECT_NE(r.body.find("II-10-9"), std::string::npos);
    EXPECT_NE(r.body.find("ДОГОВОР АРЕНДЫ"), std::string::npos);
}

TEST_F(HttpApiTest, PrintBatchContainsOnePagePerContract) {
    http_post("/tenants/add", {{"fio", "Арендатор Печати Пачки"},
                                {"activity_basis", ""},
                                {"address", ""},
                                {"inn", "1212"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", ""},
                                {"area_total_sqm", "1"},
                                {"area_trade_sqm", "1"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("Арендатор Печати Пачки");
    ASSERT_TRUE(tenant.has_value());

    http_post("/contracts/batch/create",
              {{"tenant_id", std::to_string(tenant->id)}, {"start_date", "2020-01-15"}, {"rent_amount", "21000"}});
    ContractFilter filter;
    filter.tenant_id = tenant->id;
    auto contracts = s_contracts->list(filter);
    ASSERT_EQ(contracts.size(), 3u);
    int64_t batch_id = *contracts[0].batch_id;

    auto r = http_get("/print/batch?batch_id=" + std::to_string(batch_id));
    EXPECT_EQ(r.status, 200);

    size_t count = 0;
    size_t pos = 0;
    while ((pos = r.body.find("class=\"print-page\"", pos)) != std::string::npos) {
        ++count;
        pos += 1;
    }
    EXPECT_EQ(count, 3u);
}

TEST_F(HttpApiTest, PrintSelectedReturnsOnlyChosenContracts) {
    http_post("/tenants/add", {{"fio", "Арендатор Выборочной Печати"},
                                {"activity_basis", ""},
                                {"address", ""},
                                {"inn", "1313"},
                                {"ogrn", ""},
                                {"passport_data", ""},
                                {"phone", ""},
                                {"email", ""},
                                {"room_number", ""},
                                {"area_total_sqm", "1"},
                                {"area_trade_sqm", "1"},
                                {"notes", ""}});
    auto tenant = find_tenant_by_fio("Арендатор Выборочной Печати");
    ASSERT_TRUE(tenant.has_value());

    http_post("/contracts/batch/create",
              {{"tenant_id", std::to_string(tenant->id)}, {"start_date", "2020-01-15"}, {"rent_amount", "21000"}});
    ContractFilter filter;
    filter.tenant_id = tenant->id;
    auto contracts = s_contracts->list(filter);
    ASSERT_EQ(contracts.size(), 3u);

    std::string args = "-X POST '" + kBase + "/print/selected'";
    args += " --data-urlencode 'ids=" + std::to_string(contracts[0].id) + "'";
    args += " --data-urlencode 'ids=" + std::to_string(contracts[1].id) + "'";
    auto r = run_curl(args);
    EXPECT_EQ(r.status, 200);

    size_t count = 0;
    size_t pos = 0;
    while ((pos = r.body.find("class=\"print-page\"", pos)) != std::string::npos) {
        ++count;
        pos += 1;
    }
    EXPECT_EQ(count, 2u);
}

TEST_F(HttpApiTest, UpdateSettingsPersists) {
    auto r = http_post("/settings/update", {{"org_name", "ООО «Тест»"},
                                             {"director_fio", "Директоров Директор Директорович"},
                                             {"director_basis", "Устава"},
                                             {"inn", "1234567890"},
                                             {"kpp", "123456789"},
                                             {"legal_address", "г. Тест"},
                                             {"bank_name", "Банк"},
                                             {"bank_account", "111"},
                                             {"bank_corr_account", "222"},
                                             {"bank_bik", "333"},
                                             {"mall_name", "ТестМолл"},
                                             {"mall_city", "г. Тест"},
                                             {"mall_address", "ул. Тестовая"},
                                             {"mall_floor", "3"}});
    EXPECT_EQ(r.status, 302);

    auto settings = s_settings->get();
    EXPECT_EQ(settings.org_name, "ООО «Тест»");
    EXPECT_EQ(settings.mall_name, "ТестМолл");
    EXPECT_EQ(settings.mall_floor, "3");
}

TEST_F(HttpApiTest, MissingContractReturns404) {
    auto r = http_get("/contracts/edit?id=99999999");
    EXPECT_EQ(r.status, 404);
}

TEST_F(HttpApiTest, MissingTenantReturns404) {
    auto r = http_get("/tenants/edit?id=99999999");
    EXPECT_EQ(r.status, 404);
}
