#pragma once

#include "Result.hpp"
#include "Task.hpp"

#include <glaze/forward.hpp>
#include <glaze/util/type_traits.hpp>

#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace task {

class ListTasks final {
 public:
    ListTasks() = default;
    ~ListTasks() = default;
    ListTasks(const ListTasks&) = delete;
    ListTasks& operator=(const ListTasks&) = delete;
    ListTasks(ListTasks&&) = default;
    ListTasks& operator=(ListTasks&&) = default;

    Task* Data() noexcept { return tasks_.data(); }
    [[nodiscard]] const Task* Data() const noexcept { return tasks_.data(); }
    [[nodiscard]] size_t Size() const noexcept { return tasks_.size(); }

    void AddTask(std::string name, bool done, std::optional<Date> date = {});
    void AddTask(Task task);

    tools::Result<void> Complete(size_t index) noexcept;
    tools::Result<void> Uncomplete(size_t index) noexcept;

    tools::Result<void> SetDate(size_t index, Date date) noexcept;

    [[nodiscard]]
    std::span<const Task> GetViewTasks() const noexcept {
        return std::span{tasks_};
    }

    void Print() const;
    [[nodiscard]]
    tools::Result<void> WriteToJSON(
        const std::filesystem::path& file_path) const;
    [[nodiscard]]
    tools::Result<void> WriteToYAML(
        const std::filesystem::path& file_path) const;

 private:
    std::vector<Task> tasks_;
};

}  // namespace task

namespace glz {

template <>
struct meta<task::ListTasks> {
    using T = task::ListTasks;
    // Glaze meta contract requires the member name `value`.
    // NOLINTNEXTLINE(readability-identifier-naming)
    static constexpr auto value = [](const T& self) {
        return self.GetViewTasks();
    };
};

}  // namespace glz
