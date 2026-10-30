#pragma once
#include "Error.hpp"

#include <expected>

namespace tools {

template <typename T>
using Result = std::expected<T, Error>;

}  // namespace tools
