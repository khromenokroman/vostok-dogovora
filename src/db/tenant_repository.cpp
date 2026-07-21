#include "tenant_repository.hpp"

namespace {
Tenant row_to_tenant(SQLite::Statement &q) {
    Tenant t;
    t.id = q.getColumn("id").getInt64();
    t.fio = q.getColumn("fio").getString();
    t.activity_basis = q.getColumn("activity_basis").getString();
    t.address = q.getColumn("address").getString();
    t.inn = q.getColumn("inn").getString();
    t.ogrn = q.getColumn("ogrn").getString();
    t.passport_data = q.getColumn("passport_data").getString();
    t.phone = q.getColumn("phone").getString();
    t.email = q.getColumn("email").getString();
    t.room_number = q.getColumn("room_number").getString();
    t.area_total_sqm = q.getColumn("area_total_sqm").getDouble();
    t.area_trade_sqm = q.getColumn("area_trade_sqm").getDouble();
    t.notes = q.getColumn("notes").getString();
    return t;
}
}  // namespace

std::vector<Tenant> TenantRepository::list() const {
    std::vector<Tenant> result;
    SQLite::Statement q(m_db, "SELECT * FROM tenants ORDER BY fio COLLATE NOCASE");
    while (q.executeStep()) {
        result.push_back(row_to_tenant(q));
    }
    return result;
}

std::optional<Tenant> TenantRepository::get(int64_t id) const {
    SQLite::Statement q(m_db, "SELECT * FROM tenants WHERE id = ?");
    q.bind(1, id);
    if (q.executeStep()) {
        return row_to_tenant(q);
    }
    return std::nullopt;
}

int64_t TenantRepository::create(const Tenant &tenant) const {
    SQLite::Statement q(m_db, R"sql(
INSERT INTO tenants (fio, activity_basis, address, inn, ogrn, passport_data, phone, email,
                      room_number, area_total_sqm, area_trade_sqm, notes, updated_at)
VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, datetime('now'))
)sql");
    q.bind(1, tenant.fio);
    q.bind(2, tenant.activity_basis);
    q.bind(3, tenant.address);
    q.bind(4, tenant.inn);
    q.bind(5, tenant.ogrn);
    q.bind(6, tenant.passport_data);
    q.bind(7, tenant.phone);
    q.bind(8, tenant.email);
    q.bind(9, tenant.room_number);
    q.bind(10, tenant.area_total_sqm);
    q.bind(11, tenant.area_trade_sqm);
    q.bind(12, tenant.notes);
    q.exec();
    return m_db.getLastInsertRowid();
}

void TenantRepository::update(const Tenant &tenant) const {
    SQLite::Statement q(m_db, R"sql(
UPDATE tenants SET fio = ?, activity_basis = ?, address = ?, inn = ?, ogrn = ?,
                    passport_data = ?, phone = ?, email = ?, room_number = ?,
                    area_total_sqm = ?, area_trade_sqm = ?, notes = ?, updated_at = datetime('now')
WHERE id = ?
)sql");
    q.bind(1, tenant.fio);
    q.bind(2, tenant.activity_basis);
    q.bind(3, tenant.address);
    q.bind(4, tenant.inn);
    q.bind(5, tenant.ogrn);
    q.bind(6, tenant.passport_data);
    q.bind(7, tenant.phone);
    q.bind(8, tenant.email);
    q.bind(9, tenant.room_number);
    q.bind(10, tenant.area_total_sqm);
    q.bind(11, tenant.area_trade_sqm);
    q.bind(12, tenant.notes);
    q.bind(13, tenant.id);
    q.exec();
}

bool TenantRepository::has_contracts(int64_t id) const {
    SQLite::Statement q(m_db, "SELECT COUNT(*) FROM contracts WHERE tenant_id = ?");
    q.bind(1, id);
    q.executeStep();
    return q.getColumn(0).getInt64() > 0;
}

bool TenantRepository::remove(int64_t id) const {
    if (has_contracts(id)) {
        return false;
    }
    SQLite::Statement q(m_db, "DELETE FROM tenants WHERE id = ?");
    q.bind(1, id);
    q.exec();
    return true;
}
