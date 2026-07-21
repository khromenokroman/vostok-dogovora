#pragma once

#include <optional>
#include <string>
#include <vector>

#include "db/models.hpp"

namespace pages {

std::string build_tenants_list_page(const std::vector<Tenant> &tenants, std::optional<std::string> error);

// is_edit=false -> форма добавления (POST /tenants/add), is_edit=true -> форма
// редактирования существующего арендатора (POST /tenants/update).
std::string build_tenant_form_page(const Tenant &tenant, bool is_edit, std::optional<std::string> error);

}  // namespace pages
