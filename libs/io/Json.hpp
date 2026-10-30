#pragma once

#include "ListTasks.hpp"
#include "Result.hpp"

#include <filesystem>
#include <span>

namespace io::json {

[[nodiscard]]
tools::Result<void> WriteListTasks(std::span<const task::ListTasks> list,
                                   const std::filesystem::path& direction);

}  // namespace io::json
