#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include <memory>
#include <string>

class Database {
public:
    explicit Database(const std::string &path);

    SQLite::Database &handle() { return m_db; }

private:
    void init_schema();

    SQLite::Database m_db;
};
