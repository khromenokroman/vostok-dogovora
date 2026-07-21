#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include <optional>
#include <vector>

#include "models.hpp"

class TenantRepository {
public:
    explicit TenantRepository(SQLite::Database &db) : m_db(db) {}

    std::vector<Tenant> list() const;
    std::optional<Tenant> get(int64_t id) const;
    int64_t create(const Tenant &tenant) const;
    void update(const Tenant &tenant) const;

    // Возвращает false, если у арендатора есть договоры (FOREIGN KEY RESTRICT).
    bool remove(int64_t id) const;

    bool has_contracts(int64_t id) const;

private:
    SQLite::Database &m_db;
};
