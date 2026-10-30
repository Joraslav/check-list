#include "ListTasks.hpp"

#include "Error.hpp"
#include "glaze/core/reflect.hpp"
#include "glaze/yaml/write.hpp"
#include "Result.hpp"
#include "Task.hpp"

#include <cstddef>
#include <expected>
#include <filesystem>
#include <format>
#include <optional>
#include <print>
#include <string>
#include <utility>

namespace task {

namespace fs = std::filesystem;
using namespace std::string_literals;

using tools::Error;
using tools::ErrorCode;
using tools::Result;

void ListTasks::AddTask(std::string name, bool done, std::optional<Date> date) {
    tasks_.push_back({
        .name = std::move(name),
        .done = done,
        .date = date,
    });
}

void ListTasks::AddTask(Task task) { tasks_.push_back(std::move(task)); }

Result<void> ListTasks::Complete(size_t index) noexcept {
    if (index >= tasks_.size()) {
        return std::unexpected(Error{
            .code = ErrorCode::OutOfRange,
            .message = "Index out of range"s,
        });
    }
    tasks_.at(index).done = true;
    return {};
}

Result<void> ListTasks::Uncomplete(size_t index) noexcept {
    if (index >= tasks_.size()) {
        return std::unexpected(Error{
            .code = ErrorCode::OutOfRange,
            .message = "Index out of range"s,
        });
    }
    tasks_.at(index).done = false;
    return {};
}

Result<void> ListTasks::SetDate(size_t index, Date date) noexcept {
    if (index >= tasks_.size()) {
        return std::unexpected(Error{
            .code = ErrorCode::OutOfRange,
            .message = "Index out of range"s,
        });
    }
    tasks_.at(index).date = date;
    return {};
}

void ListTasks::Print() const {
    if (tasks_.empty()) {
        std::println("(empty)");
    }
    for (size_t i = 0; i != tasks_.size(); ++i) {
        std::println("{:>2}. {}", i + 1, tasks_.at(i));
    }
}

Result<void> ListTasks::WriteToJSON(const fs::path& file_path) const {
    std::string buffer;
    if (const auto ec = glz::write_file_yaml(*this, file_path.string(), buffer);
        ec) {
        return std::unexpected(Error{
            .code = ErrorCode::Unknown,
            .message = std::format("Failed to write JSON: {}",
                                   glz::format_error(ec, buffer)),
        });
    }
    return {};
}

Result<void> ListTasks::WriteToYAML(const fs::path& file_path) const {
    std::string buffer;
    if (const auto ec = glz::write_file_yaml(*this, file_path.string(), buffer);
        ec) {
        return std::unexpected(Error{
            .code = ErrorCode::Unknown,
            .message = std::format("Failed to write YAML: {}",
                                   glz::format_error(ec, buffer)),
        });
    }
    return {};
}

}  // namespace task
