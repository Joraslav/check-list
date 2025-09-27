#include "Task.hpp"

#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

using namespace task;

void PrintHelp() {
    std::cout << "Todo List Manager\n";
    std::cout << "Usage: todo [command] [arguments]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  add <text>          Add a new task\n";
    std::cout << "  list                Show all tasks\n";
    std::cout << "  list pending        Show pending tasks\n";
    std::cout << "  list completed      Show completed tasks\n";
    std::cout << "  done <index>        Toggle task completion status\n";
    std::cout << "  remove <index>      Remove a task\n";
    std::cout << "  edit <index> <text> Edit a task\n";
    std::cout << "  config path <path>  Set the path for task files\n";
    std::cout << "  config name <name>  Set the filename for task list\n";
    std::cout << "  help                Show this help message\n";
}

int main(int argc, char** argv) {
    try {
        if (!fs::exists(DEFAULT_CONFIG_DIR) || !fs::exists(DEFAULT_OUTPUT_DIR)) {
            MakeDefaultConfig();
        }

        TaskManager manager(DEFAULT_CONFIG_DIR + "/" + DEFAULT_CONFIG_NAME);

        // Если нет аргументов, показать список задач
        if (argc < 2) {
            manager.PrintTasks();
            return 0;
        }

        std::string command = argv[1];

        if (command == "add") {
            if (argc < 3) {
                std::cerr << "Error: 'add' command requires a task description\n";
                return 1;
            }

            std::string task_text;
            for (int i = 2; i < argc; ++i) {
                if (i > 2) {
                    task_text += " ";
                }
                task_text += argv[i];
            }

            manager.AddTask(task_text);
            manager.Save();
            std::cout << "Task added successfully.\n";
        } else if (command == "list") {
            if (argc > 2) {
                std::string filter = argv[2];
                if (filter == "pending") {
                    manager.PrintTasks(false);
                } else if (filter == "completed") {
                    manager.PrintTasks(true);
                } else {
                    std::cerr << "Error: Unknown filter '" << filter
                              << "'. Use 'pending' or 'completed'.\n";
                    return 1;
                }
            } else {
                manager.PrintTasks();
            }
        } else if (command == "done") {
            if (argc < 3) {
                std::cerr << "Error: 'done' command requires a task index\n";
                return 1;
            }

            try {
                size_t index = std::stoull(argv[2]);
                if (!manager.TaskExists(index)) {
                    std::cerr << "Error: Task with index " << index << " does not exist.\n";
                    return 1;
                }
                manager.ToggleTask(index);
                manager.Save();
                std::cout << "Task status toggled successfully.\n";
            } catch (const std::invalid_argument&) {
                std::cerr << "Error: Invalid index '" << argv[2]
                          << "'. Please provide a valid number.\n";
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: " << e.what() << "\n";
                return 1;
            }
        } else if (command == "remove") {
            if (argc < 3) {
                std::cerr << "Error: 'remove' command requires a task index\n";
                return 1;
            }

            try {
                size_t index = std::stoull(argv[2]);
                if (!manager.TaskExists(index)) {
                    std::cerr << "Error: Task with index " << index << " does not exist.\n";
                    return 1;
                }
                manager.RemoveTask(index);
                manager.Save();
                std::cout << "Task removed successfully.\n";
            } catch (const std::invalid_argument&) {
                std::cerr << "Error: Invalid index '" << argv[2]
                          << "'. Please provide a valid number.\n";
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: " << e.what() << "\n";
                return 1;
            }
        } else if (command == "edit") {
            if (argc < 4) {
                std::cerr << "Error: 'edit' command requires a task index and new text\n";
                return 1;
            }

            try {
                size_t index = std::stoull(argv[2]);
                if (!manager.TaskExists(index)) {
                    std::cerr << "Error: Task with index " << index << " does not exist.\n";
                    return 1;
                }

                std::string new_text;
                for (int i = 3; i < argc; ++i) {
                    if (i > 3) new_text += " ";
                    new_text += argv[i];
                }

                manager.EditTask(index, new_text);
                manager.Save();
                std::cout << "Task edited successfully.\n";
            } catch (const std::invalid_argument&) {
                std::cerr << "Error: Invalid index '" << argv[2]
                          << "'. Please provide a valid number.\n";
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: " << e.what() << "\n";
                return 1;
            }
        } else if (command == "config") {
            if (argc < 4) {
                std::cerr << "Error: 'config' command requires 'path' or 'name' and a value\n";
                return 1;
            }

            std::string config_command = argv[2];
            std::string value = argv[3];

            if (config_command == "path") {
                TaskManager::SetPath(value, DEFAULT_CONFIG_DIR + "/" + DEFAULT_CONFIG_NAME);
                std::cout << "Path configured successfully.\n";
            } else if (config_command == "name") {
                TaskManager::SetName(value, DEFAULT_CONFIG_DIR + "/" + DEFAULT_CONFIG_NAME);
                std::cout << "Filename configured successfully.\n";
            } else {
                std::cerr << "Error: Unknown config command '" << config_command
                          << "'. Use 'path' or 'name'.\n";
                return 1;
            }
        } else if (command == "help") {
            PrintHelp();
        } else {
            std::cerr << "Error: Unknown command '" << command
                      << "'. Use 'help' to see available commands.\n";
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
