#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include "models.hpp"

class SettingsRepository {
public:
    explicit SettingsRepository(SQLite::Database &db) : m_db(db) {}

    OrganizationSettings get() const;
    void upsert(const OrganizationSettings &settings) const;

private:
    SQLite::Database &m_db;
};
