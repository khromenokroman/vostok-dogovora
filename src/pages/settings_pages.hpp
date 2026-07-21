#pragma once

#include <optional>
#include <string>

#include "db/models.hpp"

namespace pages {

std::string build_settings_page(const OrganizationSettings &settings, std::optional<std::string> notice);

}  // namespace pages
