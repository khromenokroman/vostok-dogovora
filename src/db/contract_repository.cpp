#include "contract_repository.hpp"

#include <sstream>

namespace {
void bind_opt_int64(SQLite::Statement &q, int index, const std::optional<int64_t> &value) {
    if (value.has_value()) {
        q.bind(index, *value);
    } else {
        q.bind(index);
    }
}

Contract row_to_contract(SQLite::Statement &q) {
    Contract c;
    c.id = q.getColumn("id").getInt64();
    c.tenant_id = q.getColumn("tenant_id").getInt64();
    c.rent_kopecks = q.getColumn("rent_kopecks").getInt64();
    c.start_date = q.getColumn("start_date").getString();
    c.end_date = q.getColumn("end_date").getString();
    if (!q.getColumn("batch_id").isNull()) {
        c.batch_id = q.getColumn("batch_id").getInt64();
    }
    if (!q.getColumn("batch_seq").isNull()) {
        c.batch_seq = q.getColumn("batch_seq").getInt64();
    }
    c.is_signed = q.getColumn("is_signed").getInt() != 0;
    if (!q.getColumn("signed_at").isNull()) {
        c.signed_at = q.getColumn("signed_at").getString();
    }
    return c;
}
}  // namespace

std::vector<Contract> ContractRepository::list(const ContractFilter &filter) const {
    std::ostringstream sql;
    sql << "SELECT * FROM contracts WHERE 1=1";
    if (filter.tenant_id) sql << " AND tenant_id = ?";
    if (filter.is_signed) sql << " AND is_signed = ?";
    if (filter.batch_id) sql << " AND batch_id = ?";
    sql << " ORDER BY start_date DESC, id DESC";

    SQLite::Statement q(m_db, sql.str());
    int idx = 1;
    if (filter.tenant_id) q.bind(idx++, *filter.tenant_id);
    if (filter.is_signed) q.bind(idx++, *filter.is_signed ? 1 : 0);
    if (filter.batch_id) q.bind(idx++, *filter.batch_id);

    std::vector<Contract> result;
    while (q.executeStep()) {
        result.push_back(row_to_contract(q));
    }
    return result;
}

std::optional<Contract> ContractRepository::get(int64_t id) const {
    SQLite::Statement q(m_db, "SELECT * FROM contracts WHERE id = ?");
    q.bind(1, id);
    if (q.executeStep()) {
        return row_to_contract(q);
    }
    return std::nullopt;
}

std::vector<Contract> ContractRepository::list_by_batch(int64_t batch_id) const {
    SQLite::Statement q(m_db, "SELECT * FROM contracts WHERE batch_id = ? ORDER BY batch_seq ASC");
    q.bind(1, batch_id);
    std::vector<Contract> result;
    while (q.executeStep()) {
        result.push_back(row_to_contract(q));
    }
    return result;
}

std::vector<Contract> ContractRepository::list_by_ids(const std::vector<int64_t> &ids) const {
    if (ids.empty()) {
        return {};
    }
    std::ostringstream sql;
    sql << "SELECT * FROM contracts WHERE id IN (";
    for (size_t i = 0; i < ids.size(); ++i) {
        sql << (i == 0 ? "?" : ",?");
    }
    sql << ") ORDER BY start_date ASC, id ASC";

    SQLite::Statement q(m_db, sql.str());
    for (size_t i = 0; i < ids.size(); ++i) {
        q.bind(static_cast<int>(i + 1), ids[i]);
    }
    std::vector<Contract> result;
    while (q.executeStep()) {
        result.push_back(row_to_contract(q));
    }
    return result;
}

int64_t ContractRepository::create(const Contract &contract) const {
    SQLite::Statement q(m_db, R"sql(
INSERT INTO contracts (tenant_id, rent_kopecks, start_date, end_date, batch_id, batch_seq,
                        is_signed, signed_at, updated_at)
VALUES (?, ?, ?, ?, ?, ?, ?, ?, datetime('now'))
)sql");
    q.bind(1, contract.tenant_id);
    q.bind(2, contract.rent_kopecks);
    q.bind(3, contract.start_date);
    q.bind(4, contract.end_date);
    bind_opt_int64(q, 5, contract.batch_id);
    bind_opt_int64(q, 6, contract.batch_seq);
    q.bind(7, contract.is_signed ? 1 : 0);
    if (contract.signed_at) {
        q.bind(8, *contract.signed_at);
    } else {
        q.bind(8);
    }
    q.exec();
    return m_db.getLastInsertRowid();
}

void ContractRepository::update(const Contract &contract) const {
    SQLite::Statement q(m_db, R"sql(
UPDATE contracts SET tenant_id = ?, rent_kopecks = ?, start_date = ?, end_date = ?,
                      updated_at = datetime('now')
WHERE id = ?
)sql");
    q.bind(1, contract.tenant_id);
    q.bind(2, contract.rent_kopecks);
    q.bind(3, contract.start_date);
    q.bind(4, contract.end_date);
    q.bind(5, contract.id);
    q.exec();
}

void ContractRepository::remove(int64_t id) const {
    SQLite::Statement q(m_db, "DELETE FROM contracts WHERE id = ?");
    q.bind(1, id);
    q.exec();
}

void ContractRepository::set_batch(int64_t contract_id, std::optional<int64_t> batch_id,
                                    std::optional<int64_t> batch_seq) const {
    SQLite::Statement q(m_db, "UPDATE contracts SET batch_id = ?, batch_seq = ?, updated_at = datetime('now') WHERE id = ?");
    bind_opt_int64(q, 1, batch_id);
    bind_opt_int64(q, 2, batch_seq);
    q.bind(3, contract_id);
    q.exec();
}

void ContractRepository::set_signed(int64_t contract_id, bool signed_) const {
    std::string sql = "UPDATE contracts SET is_signed = ?, signed_at = ";
    sql += signed_ ? "datetime('now')" : "NULL";
    sql += ", updated_at = datetime('now') WHERE id = ?";

    SQLite::Statement q(m_db, sql);
    q.bind(1, signed_ ? 1 : 0);
    q.bind(2, contract_id);
    q.exec();
}

int64_t ContractRepository::count_total() const {
    SQLite::Statement q(m_db, "SELECT COUNT(*) FROM contracts");
    q.executeStep();
    return q.getColumn(0).getInt64();
}

int64_t ContractRepository::count_unsigned() const {
    SQLite::Statement q(m_db, "SELECT COUNT(*) FROM contracts WHERE is_signed = 0");
    q.executeStep();
    return q.getColumn(0).getInt64();
}
