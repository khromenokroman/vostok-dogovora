#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

#include <optional>
#include <vector>

#include "models.hpp"

struct ContractFilter {
    std::optional<int64_t> tenant_id;
    std::optional<bool> is_signed;
    std::optional<int64_t> batch_id;
};

class ContractRepository {
public:
    explicit ContractRepository(SQLite::Database &db) : m_db(db) {}

    std::vector<Contract> list(const ContractFilter &filter = {}) const;
    std::optional<Contract> get(int64_t id) const;
    std::vector<Contract> list_by_batch(int64_t batch_id) const;
    std::vector<Contract> list_by_ids(const std::vector<int64_t> &ids) const;

    int64_t create(const Contract &contract) const;
    void update(const Contract &contract) const;
    void remove(int64_t id) const;

    void set_batch(int64_t contract_id, std::optional<int64_t> batch_id, std::optional<int64_t> batch_seq) const;
    void set_signed(int64_t contract_id, bool signed_) const;

    int64_t count_total() const;
    int64_t count_unsigned() const;

private:
    SQLite::Database &m_db;
};
