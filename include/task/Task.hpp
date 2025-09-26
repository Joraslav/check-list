#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace task {

namespace fs = std::filesystem;

using std::string_literals::operator""s;

const std::string CONFIG_DIR = "../config"s;
const std::string CONFIG_NAME = "config_todo.json"s;

const std::string OUTPUT_DIR = "../notepad"s;
const std::string DEFAULT_LIST = "checklist.json"s;

struct Task {
    std::string text;
    bool done = false;

    Task() = default;
    Task(const std::string& text_task, bool done_task) : text(text_task), done(done_task) {}
};

class TaskManager {
 public:
    using json = nlohmann::json;

    TaskManager();

    void LoadTasksFromFile(const std::string& filename);
    void AddTask(const std::string& text);
    void AddTask(const Task& task);
    void ToggleTask(size_t index);
    void RemoveTask(size_t index);
    void EditTask(size_t index, const std::string& new_text);
    bool TaskExists(size_t index) const;
    void Save() const;
    
    // Методы для получения информации
    const std::vector<Task>& GetTasks() const;
    void PrintTasks() const;
    void PrintTasks(bool only_completed) const;

    static void SetPath(const std::string& path);
    static void SetName(const std::string& name);

 private:
    std::vector<Task> tasks_;
    std::string path_;
    std::string filename_;
    std::string full_name_;
};

void MakeDefaultConfig();

}  // namespace task
