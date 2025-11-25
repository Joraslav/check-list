#include "Parser.hpp"
#include "Task.hpp"

#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

using namespace task;
using namespace parser;

void PrintHelp() {
    std::cout << "Todo List Manager\n";
    std::cout << "Usage: todo [command] [arguments]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  add <text>          Add a new task\n";
    std::cout << "  list                Show all tasks\n";
    std::cout << "  list pending        Show pending tasks\n";
    std::cout << "  list completed      Show completed tasks\n";
    std::cout << "  clear               Clear all tasks\n";
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

        // Обработка и действие в зависимости от команды
        std::string command = ToLower(std::string(argv[1]));
        Parser parser_obj;
        parser_obj.Parse(argc, argv);
        TypeCommand type_command = parser_obj.GetTypeCommand();
        switch (type_command) {
            case parser::TypeCommand::ADD: {
                manager.AddTask(parser_obj.GetTaskText());
                manager.Save();
                std::cout << "Task added successfully.\n";
                break;
            }
            case parser::TypeCommand::LIST: {
                try {
                    if (auto pList = std::get_if<parser::ListOption>(&parser_obj.GetCommandOption())) {
                        const auto& list_option = *pList;
                        switch (list_option) {
                            case parser::ListOption::PENDING: {
                                manager.PrintTasks(false);
                                break;
                            }
                            case parser::ListOption::COMPLETED: {
                                manager.PrintTasks(true);
                                break;
                            }
                            default:
                                throw std::invalid_argument(" Unknown option list.");
                                break;
                        }
                    } else {
                        manager.PrintTasks();
                    }
                    break;
                } catch (const std::invalid_argument& list_ex) {
                    std::cerr << "Error: " << list_ex.what() << '\n';
                    return 1;
                }

                break;
            }
            case parser::TypeCommand::CLEAR: {
                manager.ClearTasks();
                manager.Save();
                std::cout << "All tasks in " << manager.GetFullName() << " cleared successfully\n";
                break;
            }
            case parser::TypeCommand::DONE: {
                try {
                    auto index_opt = parser_obj.GetTaskIndex();
                    if (!index_opt.has_value()) {
                        throw std::invalid_argument(" Invalid index.\n");
                    }

                    size_t index = index_opt.value();

                    if (!manager.TaskExists(index)) {
                        throw std::invalid_argument(" Index " + std::to_string(index) +
                                                    " does not exist.\n");
                    }

                    manager.ToggleTask(index);
                    manager.Save();
                    std::cout << "Task status toggled successfully.\n";
                    break;
                } catch (const std::invalid_argument& done_ex) {
                    std::cerr << "Error: " << done_ex.what() << '\n';
                    return 1;
                }
            }
            case parser::TypeCommand::REMOVE: {
                try {
                    auto index_opt = parser_obj.GetTaskIndex();
                    if (!index_opt.has_value()) {
                        throw std::invalid_argument(" Invalid index.\n");
                    }

                    size_t index = index_opt.value();

                    if (!manager.TaskExists(index)) {
                        throw std::invalid_argument(" Index " + std::to_string(index) +
                                                    " does not exist.\n");
                    }

                    manager.RemoveTask(index);
                    manager.Save();
                    std::cout << "Task removed successfully.\n";
                    break;
                } catch (const std::invalid_argument& remove_ex) {
                    std::cerr << "Error: " << remove_ex.what() << '\n';
                    return 1;
                }
            }
            case parser::TypeCommand::EDIT: {
                try {
                    auto index_opt = parser_obj.GetTaskIndex();
                    if (!index_opt.has_value()) {
                        throw std::invalid_argument(" Invalid index.\n");
                    }

                    size_t index = index_opt.value();

                    if (!manager.TaskExists(index)) {
                        throw std::invalid_argument(" Index " + std::to_string(index) +
                                                    " does not exist.\n");
                    }

                    manager.EditTask(index, parser_obj.GetTaskText());
                    manager.Save();
                    std::cout << "Task edited successfully.\n";
                    break;
                } catch (const std::invalid_argument& edit_ex) {
                    std::cerr << "Error: " << edit_ex.what() << '\n';
                    return 1;
                }
            }
            case parser::TypeCommand::HELP: {
                PrintHelp();
                break;
            }
            case parser::TypeCommand::CONFIG: {
                try {
                    if (auto pConfig = std::get_if<parser::ConfigOption>(&parser_obj.GetCommandOption())) {
                        const auto& config_option = *pConfig;
                        switch (config_option) {
                            case parser::ConfigOption::PATH: {
                                TaskManager::SetPath(
                                    parser_obj.GetTaskText(),
                                    DEFAULT_CONFIG_DIR + "/" + DEFAULT_CONFIG_NAME);
                                std::cout << "Path configured successfully.\n";
                                break;
                            }
                            case parser::ConfigOption::NAME: {
                                TaskManager::SetName(
                                    parser_obj.GetTaskText(),
                                    DEFAULT_CONFIG_DIR + "/" + DEFAULT_CONFIG_NAME);
                                std::cout << "Filename configured successfully.\n";
                                break;
                            }
                            default:
                                throw std::invalid_argument(" Unknown option config.");
                                break;
                        }
                    } else {
                        throw std::invalid_argument(" Unknown option config.");
                    }
                    break;
                } catch (const std::invalid_argument& config_ex) {
                    std::cerr << "Error: " << config_ex.what() << '\n';
                    return 1;
                }
            }
            default: {
                std::cerr << "Error: Unknown command '" << command
                          << "'. Use 'help' to see available commands.\n";
                return 1;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
