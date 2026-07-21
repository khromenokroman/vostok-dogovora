#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "db/models.hpp"

namespace pages {

// Один печатный лист (весь текст договора) для указанного договора/арендатора.
std::string build_contract_print_fragment(const Contract &contract, const Tenant &tenant,
                                           const OrganizationSettings &org);

// Оборачивает один или несколько печатных листов в готовый HTML-документ с тулбаром
// "Назад"/"Печать" — используется как для одиночной печати, так и для пачки/выборки.
std::string build_print_document(std::string_view title,
                                  const std::vector<std::pair<Contract, Tenant>> &items,
                                  const OrganizationSettings &org);

}  // namespace pages
