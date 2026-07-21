#pragma once

#include <nlohmann/json.hpp>
#include <string>

struct Config {
    int port = 8080;
    std::string host = "0.0.0.0";
    std::string db_path = "db/vostok.sqlite3";
    std::string log_level = "info";
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config, port, host, db_path, log_level)

Config load_config(const std::string &path);
