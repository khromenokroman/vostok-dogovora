#include "database.hpp"

#include <filesystem>

Database::Database(const std::string &path)
    : m_db(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    m_db.exec("PRAGMA foreign_keys = ON");
    init_schema();
}

void Database::init_schema() {
    m_db.exec(R"sql(
CREATE TABLE IF NOT EXISTS organization_settings (
    id                  INTEGER PRIMARY KEY CHECK (id = 1),
    org_name            TEXT NOT NULL DEFAULT '',
    director_fio        TEXT NOT NULL DEFAULT '',
    director_basis      TEXT NOT NULL DEFAULT '',
    inn                 TEXT NOT NULL DEFAULT '',
    kpp                 TEXT NOT NULL DEFAULT '',
    legal_address       TEXT NOT NULL DEFAULT '',
    bank_name           TEXT NOT NULL DEFAULT '',
    bank_account        TEXT NOT NULL DEFAULT '',
    bank_corr_account   TEXT NOT NULL DEFAULT '',
    bank_bik            TEXT NOT NULL DEFAULT '',
    mall_name           TEXT NOT NULL DEFAULT '',
    mall_city           TEXT NOT NULL DEFAULT '',
    mall_address        TEXT NOT NULL DEFAULT '',
    mall_floor          TEXT NOT NULL DEFAULT '',
    updated_at          TEXT NOT NULL DEFAULT (datetime('now'))
)
)sql");

    m_db.exec(R"sql(
CREATE TABLE IF NOT EXISTS tenants (
    id                  INTEGER PRIMARY KEY AUTOINCREMENT,
    fio                 TEXT NOT NULL,
    activity_basis      TEXT NOT NULL DEFAULT '',
    address             TEXT NOT NULL DEFAULT '',
    inn                 TEXT NOT NULL DEFAULT '',
    ogrn                TEXT NOT NULL DEFAULT '',
    passport_data       TEXT NOT NULL DEFAULT '',
    phone               TEXT NOT NULL DEFAULT '',
    email               TEXT NOT NULL DEFAULT '',
    room_number         TEXT NOT NULL DEFAULT '',
    area_total_sqm      REAL NOT NULL DEFAULT 0,
    area_trade_sqm      REAL NOT NULL DEFAULT 0,
    notes               TEXT NOT NULL DEFAULT '',
    created_at          TEXT NOT NULL DEFAULT (datetime('now')),
    updated_at          TEXT NOT NULL DEFAULT (datetime('now'))
)
)sql");

    m_db.exec(R"sql(
CREATE TABLE IF NOT EXISTS contracts (
    id                  INTEGER PRIMARY KEY AUTOINCREMENT,
    tenant_id           INTEGER NOT NULL REFERENCES tenants(id) ON DELETE RESTRICT,
    rent_kopecks        INTEGER NOT NULL,
    start_date          TEXT NOT NULL,
    end_date            TEXT NOT NULL,
    batch_id            INTEGER NULL REFERENCES contracts(id),
    batch_seq           INTEGER NULL,
    is_signed           INTEGER NOT NULL DEFAULT 0,
    signed_at           TEXT NULL,
    created_at          TEXT NOT NULL DEFAULT (datetime('now')),
    updated_at          TEXT NOT NULL DEFAULT (datetime('now'))
)
)sql");

    m_db.exec("CREATE INDEX IF NOT EXISTS idx_contracts_tenant ON contracts(tenant_id)");
    m_db.exec("CREATE INDEX IF NOT EXISTS idx_contracts_batch ON contracts(batch_id)");
    m_db.exec("CREATE INDEX IF NOT EXISTS idx_contracts_signed ON contracts(is_signed)");

    m_db.exec(R"sql(
INSERT OR IGNORE INTO organization_settings (id, org_name, mall_name, mall_city)
VALUES (1, '', '', '')
)sql");
}
