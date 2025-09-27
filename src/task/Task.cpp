#include "Task.hpp"

#include <format>
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace task {

TaskManager::TaskManager(const std::string& config_path) : config_path_(config_path) {
    std::ifstream fin(config_path_);
    if (!fin) {
        const std::string error_message = std::format(
            "Failed to open config file: {}\nSelect config file with command: todo config "
            "<path-to-dir>",
            config_path_);
        throw std::runtime_error(error_message);
    }

    json j = json::parse(fin);

    path_ = static_cast<std::string>(j["path"]);
    filename_ = static_cast<std::string>(j["name"]);

    fin.close();

    full_name_ = path_ + "/" + filename_;
    LoadTasksFromFile(full_name_);
}

void TaskManager::LoadTasksFromFile(const std::string& filename) {
    if (!fs::exists(filename)) {
        tasks_.clear();
        return;
    }
    
    std::ifstream fin(filename);
    if (!fin) {
        const std::string error_message = std::format(
            "Failed to open list file: {}\nSelect: todo list <name-your-todo-list>", filename);
        throw std::runtime_error(error_message);
    }

    // Проверяем, что файл не пустой
    fin.seekg(0, std::ios::end);
    size_t file_size = fin.tellg();
    fin.seekg(0, std::ios::beg);
    
    if (file_size == 0) {
        // Если файл пустой, очищаем список задач и возвращаемся
        tasks_.clear();
        fin.close();
        return;
    }

    json j = json::parse(fin);
    
    // Проверяем, что JSON является массивом
    if (!j.is_array()) {
        fin.close();
        const std::string error_message = std::format(
            "Invalid file format in {}: expected an array of tasks", filename);
        throw std::runtime_error(error_message);
    }

    tasks_.clear();
    for (const auto& task : j) {
        // Проверяем, что каждый элемент является объектом
        if (!task.is_object()) {
            fin.close();
            const std::string error_message = std::format(
                "Invalid task format in {}: task must be a JSON object", filename);
            throw std::runtime_error(error_message);
        }
        
        // Проверяем, что каждый элемент имеет необходимые поля
        if (!task.contains("text") || !task.contains("done")) {
            fin.close();
            const std::string error_message = std::format(
                "Invalid task format in {}: missing 'text' or 'done' field", filename);
            throw std::runtime_error(error_message);
        }
        
        // Проверяем типы полей
        if (!task["text"].is_string()) {
            fin.close();
            const std::string error_message = std::format(
                "Invalid task format in {}: 'text' field must be a string", filename);
            throw std::runtime_error(error_message);
        }
        
        if (!task["done"].is_boolean()) {
            fin.close();
            const std::string error_message = std::format(
                "Invalid task format in {}: 'done' field must be a boolean", filename);
            throw std::runtime_error(error_message);
        }
        
        tasks_.push_back(Task(task["text"], task["done"]));
    }

    fin.close();
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

bool TaskManager::TaskExists(size_t index) const {
    return index < tasks_.size();
}

void TaskManager::RemoveTask(size_t index) {
    if (index >= tasks_.size()) {
        const std::string error_message = std::format(
            "Task with index {} does not exist.\nRecheck list and choose different task index",
            index);
        throw std::out_of_range(error_message);
    }
    tasks_.erase(tasks_.begin() + index);
}

void TaskManager::EditTask(size_t index, const std::string& new_text) {
    if (index >= tasks_.size()) {
        const std::string error_message = std::format(
            "Task with index {} does not exist.\nRecheck list and choose different task index",
            index);
        throw std::out_of_range(error_message);
    }
    tasks_[index].text = new_text;
}

const std::vector<Task>& TaskManager::GetTasks() const {
    return tasks_;
}

void TaskManager::PrintTasks() const {
    if (tasks_.empty()) {
        std::cout << "No tasks found.\n";
        return;
    }
    
    for (size_t i = 0; i != tasks_.size(); ++i) {
        std::cout << i << ". ";
        if (tasks_[i].done) {
            std::cout << "[x] ";
        } else {
            std::cout << "[ ] ";
        }
        std::cout << tasks_[i].text << "\n";
    }
}

void TaskManager::PrintTasks(bool only_completed) const {
    if (tasks_.empty()) {
        std::cout << "No tasks found.\n";
        return;
    }
    
    bool found = false;
    for (size_t i = 0; i < tasks_.size(); ++i) {
        if (only_completed == tasks_[i].done) {
            std::cout << i << ". ";
            if (tasks_[i].done) {
                std::cout << "[x] ";
            } else {
                std::cout << "[ ] ";
            }
            std::cout << tasks_[i].text << "\n";
            found = true;
        }
    }
    
    if (!found) {
        if (only_completed) {
            std::cout << "No completed tasks found.\n";
        } else {
            std::cout << "No pending tasks found.\n";
        }
    }
}

void TaskManager::Save() const {
    json j = json::array();
    for (const Task& task : tasks_) {
        j.push_back({{"text", task.text}, {"done", task.done}});
    }

    // Проверяем существование директории и создаем её при необходимости
    const fs::path dir_path = fs::path(full_name_).parent_path();
    if (!dir_path.empty() && !fs::exists(dir_path)) {
        std::error_code ec;
        fs::create_directories(dir_path, ec);
        if (ec) {
            const std::string error_message =
                std::format("Failed to create directory {}: {}", dir_path.string(), ec.message());
            throw std::runtime_error(error_message);
        }
    }

    std::ofstream fout(full_name_);
    if (!fout) {
        const std::string error_message =
            std::format("Failed to open file for writing: {}", full_name_);
        throw std::runtime_error(error_message);
    }
    fout << j.dump(4);
    fout.close();
    
    // Проверяем, что файл был успешно записан
    if (fout.fail()) {
        const std::string error_message =
            std::format("Failed to write data to file: {}", full_name_);
        throw std::runtime_error(error_message);
    }
}

void TaskManager::SetPath(const std::string& path, const std::string& config_path) {
    // Проверяем существование директории конфигурации и создаем её при необходимости
    const fs::path config_dir = fs::path(config_path).parent_path();
    if (!config_dir.empty() && !fs::exists(config_dir)) {
        std::error_code ec;
        fs::create_directories(config_dir, ec);
        if (ec) {
            const std::string error_message =
                "Failed to create config directory " + config_dir.string() + ": " + ec.message();
            throw std::runtime_error(error_message);
        }
    }
    
    std::ifstream fin(config_path);
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

    std::ofstream fout(config_path);
    if (!fout) {
        const fs::path dir_path = fs::path(config_path);
        const std::string error_message = "Failed to open config file for writing: " +
                                          fs::absolute(dir_path).string();
        throw std::runtime_error(error_message);
    }
    fout << j.dump(4);
    fout.close();
    
    // Проверяем, что файл был успешно записан
    if (fout.fail()) {
        const std::string error_message =
            "Failed to write data to config file: " + config_path;
        throw std::runtime_error(error_message);
    }
}

void TaskManager::SetName(const std::string& name, const std::string& config_path) {
    // Проверяем существование директории конфигурации и создаем её при необходимости
    const fs::path config_dir = fs::path(config_path).parent_path();
    if (!config_dir.empty() && !fs::exists(config_dir)) {
        std::error_code ec;
        fs::create_directories(config_dir, ec);
        if (ec) {
            const std::string error_message =
                "Failed to create config directory " + config_dir.string() + ": " + ec.message();
            throw std::runtime_error(error_message);
        }
    }
    
    std::ifstream fin(config_path);
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

    std::ofstream fout(config_path);
    if (!fout) {
        const fs::path dir_path = fs::path(config_path);
        const std::string error_message = "Failed to open config file for writing: " +
                                          fs::absolute(dir_path).string();
        throw std::runtime_error(error_message);
    }
    fout << j.dump(4);
    fout.close();
    
    // Проверяем, что файл был успешно записан
    if (fout.fail()) {
        const std::string error_message =
            "Failed to write data to config file: " + config_path;
        throw std::runtime_error(error_message);
    }
}

// ------- Функции -------

void MakeDefaultConfig() {
    using nlohmann::json;

    // Создаем директории с проверкой ошибок
    std::error_code ec;
    fs::create_directory(DEFAULT_CONFIG_DIR, ec);
    if (ec) {
        const std::string error_message =
            "Failed to create config directory " + DEFAULT_CONFIG_DIR + ": " + ec.message();
        throw std::runtime_error(error_message);
    }
    
    fs::create_directory(DEFAULT_OUTPUT_DIR, ec);
    if (ec) {
        const std::string error_message =
            "Failed to create output directory " + DEFAULT_OUTPUT_DIR + ": " + ec.message();
        throw std::runtime_error(error_message);
    }
    
    const std::string config_name = DEFAULT_CONFIG_DIR + "/" + DEFAULT_CONFIG_NAME;

    std::ofstream fout(config_name);
    if (!fout) {
        const std::string error_message =
            "Failed to create config file: " + config_name;
        throw std::runtime_error(error_message);
    }

    json j = json::object();
    j["path"] = DEFAULT_OUTPUT_DIR;
    j["name"] = "checklist.json"s;

    fout << j.dump(4);
    fout.close();
    
    // Проверяем, что файл был успешно записан
    if (fout.fail()) {
        const std::string error_message =
            "Failed to write data to config file: " + config_name;
        throw std::runtime_error(error_message);
    }
}

}  // namespace task
