#include "Task.hpp"

#include <format>
#include <fstream>
#include <stdexcept>

namespace task {

TaskManager::TaskManager(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin) {
        const std::string error_message = std::format(
            "Failed to open file: {}\nSelect file with command: todo file <path-to-file>",
            filename);
        throw std::runtime_error(error_message);
    }

    json j = json::parse(fin);

    for (const auto& item : j) {
        tasks_.push_back({item["text"], item["done"]});
    }
}

void TaskManager::AddTask(const std::string& text) { tasks_.emplace_back(text, false); }

void TaskManager::AddTask(const Task& task) { tasks_.emplace_back(task); }

void TaskManager::ToggleTask(size_t index) {
    if (index >= tasks_.size()) {
        const std::string error_message = std::format(
            "Task with index {} does not exist.\nRecheck list and choose different task index",
            index);
        throw std::out_of_range(error_message);
    }
    tasks_[index].done = !tasks_[index].done;
}

void TaskManager::Save() const {
    json j = json::array();
    for (const Task& task : tasks_) {
        j.push_back({{"text", task.text}, {"done", task.done}});
    }

    std::ofstream fout(filename_);
    if (!fout) {
        const std::string error_message =
            std::format("Failed to open file for writing: {}", filename_);
        throw std::runtime_error(error_message);
    }
    fout << j.dump(4);
}

void TaskManager::SetPath(const std::string& path) {
    std::ifstream fin(CONFIG_NAME);
    json j;

    if (fin) {
        try {
            j = json::parse(fin);
        } catch (const json::parse_error& e) {
            j = json::object();
        }
    } else {
        j = json::object();
    }
    fin.close();

    j["path"] = path;

    std::ofstream fout(CONFIG_NAME);
    if (!fout) {
        const fs::path dir_path = fs::path(CONFIG_NAME);
        const std::string error_message = std::format("Failed to open config file for writing: {}",
                                                      fs::absolute(dir_path).string());
        throw std::runtime_error(error_message);
    }
    fout << j.dump(4);
    fout.close();
}

void TaskManager::SetName(const std::string& name) {
    std::ifstream fin(CONFIG_NAME);
    json j;

    if (fin) {
        try {
            j = json::parse(fin);
        } catch (const json::parse_error& e) {
            j = json::object();
        }
    } else {
        j = json::object();
    }
    fin.close();

    j["name"] = name;

    std::ofstream fout(CONFIG_NAME);
    if (!fout) {
        const fs::path dir_path = fs::path(CONFIG_NAME);
        const std::string error_message = std::format("Failed to open config file for writing: {}",
                                                      fs::absolute(dir_path).string());
        throw std::runtime_error(error_message);
    }
    fout << j.dump(4);
    fout.close();
}

// ------- Функции -------
void MakeDefaultConfig(const std::string& dir, const std::string& name) {
    using nlohmann::json;

    fs::create_directory(dir);
    const std::string config_name = dir + CONFIG_NAME;

    std::ofstream fout(config_name);

    json j = json::object();
    j["path"] = OUTPUT_DIR;
    j["name"] = name;

    fout << j.dump(4);
    fout.close();
}

}  // namespace task
