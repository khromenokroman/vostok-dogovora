cmake_minimum_required(VERSION 3.25)
project(vostok_dogovora VERSION 0.2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(httplib REQUIRED IMPORTED_TARGET cpp-httplib)

add_executable(vostok_dogovora
        main.cpp
        src/config.cpp
        src/db/database.cpp
        src/db/tenant_repository.cpp
        src/db/contract_repository.cpp
        src/db/settings_repository.cpp
        src/business/number_to_words.cpp
        src/business/contract_batch.cpp
        src/html/format.cpp
        src/html/layout.cpp
        src/pages/tenants_pages.cpp
        src/pages/contracts_pages.cpp
        src/pages/print_pages.cpp
        src/pages/settings_pages.cpp
        src/app_core.cpp
        src/app_tenants.cpp
        src/app_contracts.cpp
        src/app_batch.cpp
        src/app_print.cpp
        src/app_settings.cpp
)

target_include_directories(vostok_dogovora PRIVATE src)
target_link_libraries(vostok_dogovora PRIVATE
        fmt::fmt
        nlohmann_json::nlohmann_json
        SQLiteCpp
        PkgConfig::httplib
        Threads::Threads
)
