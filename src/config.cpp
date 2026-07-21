#include "config.hpp"

#include <fstream>
#include <iostream>

Config load_config(const std::string &path) {
    Config cfg;
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Не могу открыть конфиг(" << path << "), используются значения по умолчанию" << std::endl;
        return cfg;
    }

    nlohmann::json j;
    file >> j;

    if (j.contains("port")) cfg.port = j.value("port", cfg.port);
    if (j.contains("host")) cfg.host = j.value("host", cfg.host);
    if (j.contains("db_path")) cfg.db_path = j.value("db_path", cfg.db_path);
    if (j.contains("log_level")) cfg.log_level = j.value("log_level", cfg.log_level);

    return cfg;
}
