#pragma once

#include "glaze/core/common.hpp"
#include "glaze/forward.hpp"

#include <chrono>
#include <format>
#include <optional>
#include <string>

namespace task {

using Date = std::chrono::sys_seconds;

struct Task final {
    std::string name;
    bool done;
    std::optional<Date> date;
};

}  // namespace task

namespace glz {

template <>
struct meta<task::Task> {
    using T = task::Task;
    // Glaze meta contract requires the member name `value`.
    // NOLINTNEXTLINE(readability-identifier-naming)
    [[maybe_unused]] static constexpr auto value =
        object("name", &T::name, "is_done", &T::done, "date", &T::date);
};

}  // namespace glz

template <>
struct std::formatter<task::Task> {
    static constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    static auto format(const task::Task& t, std::format_context& ctx) {
        const char mark = t.done ? 'x' : ' ';
        if (t.date.has_value()) {
            return std::format_to(ctx.out(), "[{}] {} ({:%Y-%m-%d %H:%M:%S})",
                                  mark, t.name, *t.date);
        }
        return std::format_to(ctx.out(), "[{}] {}", mark, t.name);
    }
};
