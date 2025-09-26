#include "Task.hpp"

#include <format>
#include <fstream>
#include <stdexcept>

namespace task {

TaskManager::TaskManager(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin) {
        const std::string error_message = std::format(
            "Failed to open file: {}\nSelect file with command: todo file <path-to-file>"s,
            filename);
        throw std::runtime_error(error_message);
    }

    json j;
    fin >> j;

    for (const auto& item : j) {
        tasks_.push_back({item["text"], item["done"]});
    }
}

void TaskManager::AddTask(const std::string& text) { tasks_.push_back({std::move(text), false}); }

void TaskManager::AddTask(const Task& task) { tasks_.push_back(task); }

void TaskManager::ToggleTask(size_t index) {
    if (index > tasks_.size()) {
        const std::string error_message = std::format(
            "Task with index {} does not exist.\nRecheck list and choose disserent task index"s,
            index);
        throw std::out_of_range(error_message);
    }
}

}  // namespace task
