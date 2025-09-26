#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace task {

using std::string_literals::operator""s;

struct Task {
    std::string text;
    bool done = false;
};

class TaskManager {
 public:
    using json = nlohmann::json;

    TaskManager(const std::string& filename = "checklist.json"s);

    void AddTask(const std::string& text);
    void AddTask(const Task& task);
    void ToggleTask(size_t index);

 private:
    std::vector<Task> tasks_;
    std::string filename_ = "checklist.json"s;
};

}  // namespace task
