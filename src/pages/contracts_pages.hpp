#pragma once

#include <optional>
#include <string>
#include <vector>

#include "business/contract_batch.hpp"
#include "db/models.hpp"

namespace pages {

struct ContractRow {
    Contract contract;
    Tenant tenant;
};

struct ContractsListFilter {
    std::optional<int64_t> tenant_id;
    std::optional<bool> is_signed;
};

std::string build_contracts_list_page(const std::vector<ContractRow> &rows, const std::vector<Tenant> &all_tenants,
                                       const ContractsListFilter &filter, std::optional<std::string> error);

// is_edit=false -> форма добавления одного договора (POST /contracts/add),
// is_edit=true -> форма редактирования (POST /contracts/update).
std::string build_contract_form_page(const Contract &contract, const std::vector<Tenant> &all_tenants, bool is_edit,
                                      std::optional<std::string> error);

// Форма создания пачки договоров по арендатору. Если periods не пуст — под формой
// показывается превью разбивки (даты и число договоров) перед подтверждением.
std::string build_batch_form_page(const Tenant &tenant, const std::string &start_date, const std::string &rent_amount,
                                   const std::vector<contract_batch::BatchPeriod> &periods,
                                   std::optional<std::string> error);

}  // namespace pages
