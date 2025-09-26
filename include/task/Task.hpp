#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace task {

namespace fs = std::filesystem;

using std::string_literals::operator""s;

const std::string CONFIG_NAME = "/config_todo.json"s;
const std::string OUTPUT_DIR = "../config"s;

struct Task {
    std::string text;
    bool done = false;

    Task() = default;
    Task(const std::string& text_task, bool done_task) : text(text_task), done(done_task) {}
};

class TaskManager {
 public:
    using json = nlohmann::json;

    TaskManager(const std::string& filename = "checklist.json"s);

    void AddTask(const std::string& text);
    void AddTask(const Task& task);
    void ToggleTask(size_t index);
    void Save() const;

    static void SetPath(const std::string& path);
    static void SetName(const std::string& name);

 private:
    std::vector<Task> tasks_;
    fs::path path_;
    std::string filename_;
};

void MakeDefaultConfig(const std::string& dir = OUTPUT_DIR,
                       const std::string& name = "checklist.json"s);

}  // namespace task
