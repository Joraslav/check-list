#include "Config.hpp"
#include "ListTasks.hpp"
#include "Task.hpp"

#include <exception>
#include <optional>
#include <print>

using namespace task;
using namespace tools;
using namespace std::string_literals;

int main() {
    try {
        ConfigManager cfg;
        const auto path = cfg.GetStoragePath().string();
        std::println("Path storage {}", path);
        ListTasks list;
        list.AddTask(Task{
            .name = "Make sex"s,
            .done = false,
            .date = std::nullopt,
        });

        list.Print();
    } catch (const std::exception& e) {
        std::println("Error catch: {}", e.what());
    }
    return 0;
}
