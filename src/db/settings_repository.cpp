#include "settings_repository.hpp"

OrganizationSettings SettingsRepository::get() const {
    SQLite::Statement q(m_db, "SELECT * FROM organization_settings WHERE id = 1");
    OrganizationSettings s;
    if (q.executeStep()) {
        s.org_name = q.getColumn("org_name").getString();
        s.director_fio = q.getColumn("director_fio").getString();
        s.director_basis = q.getColumn("director_basis").getString();
        s.inn = q.getColumn("inn").getString();
        s.kpp = q.getColumn("kpp").getString();
        s.legal_address = q.getColumn("legal_address").getString();
        s.bank_name = q.getColumn("bank_name").getString();
        s.bank_account = q.getColumn("bank_account").getString();
        s.bank_corr_account = q.getColumn("bank_corr_account").getString();
        s.bank_bik = q.getColumn("bank_bik").getString();
        s.mall_name = q.getColumn("mall_name").getString();
        s.mall_city = q.getColumn("mall_city").getString();
        s.mall_address = q.getColumn("mall_address").getString();
        s.mall_floor = q.getColumn("mall_floor").getString();
    }
    return s;
}

void SettingsRepository::upsert(const OrganizationSettings &settings) const {
    SQLite::Statement q(m_db, R"sql(
INSERT INTO organization_settings (id, org_name, director_fio, director_basis, inn, kpp,
                                    legal_address, bank_name, bank_account, bank_corr_account,
                                    bank_bik, mall_name, mall_city, mall_address, mall_floor, updated_at)
VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, datetime('now'))
ON CONFLICT(id) DO UPDATE SET
    org_name = excluded.org_name,
    director_fio = excluded.director_fio,
    director_basis = excluded.director_basis,
    inn = excluded.inn,
    kpp = excluded.kpp,
    legal_address = excluded.legal_address,
    bank_name = excluded.bank_name,
    bank_account = excluded.bank_account,
    bank_corr_account = excluded.bank_corr_account,
    bank_bik = excluded.bank_bik,
    mall_name = excluded.mall_name,
    mall_city = excluded.mall_city,
    mall_address = excluded.mall_address,
    mall_floor = excluded.mall_floor,
    updated_at = datetime('now')
)sql");
    q.bind(1, settings.org_name);
    q.bind(2, settings.director_fio);
    q.bind(3, settings.director_basis);
    q.bind(4, settings.inn);
    q.bind(5, settings.kpp);
    q.bind(6, settings.legal_address);
    q.bind(7, settings.bank_name);
    q.bind(8, settings.bank_account);
    q.bind(9, settings.bank_corr_account);
    q.bind(10, settings.bank_bik);
    q.bind(11, settings.mall_name);
    q.bind(12, settings.mall_city);
    q.bind(13, settings.mall_address);
    q.bind(14, settings.mall_floor);
    q.exec();
}
