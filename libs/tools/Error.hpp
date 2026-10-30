#pragma once

#include <array>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>
#include <utility>

namespace tools {

enum class ErrorCode : uint8_t {
    NotFounded,
    OutOfRange,
    Parse,
    Unknown,
};

struct Error final {
    ErrorCode code;
    std::string message;
};

}  // namespace tools

template <>
struct std::formatter<tools::ErrorCode> : std::formatter<std::string_view> {
    auto format(tools::ErrorCode ec, std::format_context& ctx) const {
        constexpr auto names = std::to_array<std::string_view>({
            "NotFounded",
            "OutOfRange",
            "Parse"
            "Unknown",
        });
        auto idx = std::to_underlying(ec);
        std::string_view name =
            (idx < names.size()) ? names.at(idx) : "Unknown";
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct std::formatter<tools::Error> {
    static constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    static auto format(const tools::Error& err, std::format_context& ctx) {
        return std::format_to(ctx.out(), "[{}]: {}", err.code, err.message);
    }
};
