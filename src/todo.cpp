#include "Parser.hpp"
#include "Task.hpp"

#include <filesystem>
#include <format>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

using namespace task;
using namespace parser;
using CommandOption = Parser::CommandOption;

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

        Parser parser;
        parser.Parse(argc, argv);

        const TypeCommand type_command = parser.GetTypeCommand();
        switch (type_command) {
            case TypeCommand::HELP:
                manager.PrintTasks();
                break;
            case TypeCommand::ADD:
                manager.AddTask(parser.GetTaskText());
                manager.Save();
                std::cout << "Task added successfully.\n";
                break;
            case TypeCommand::LIST: {
                const CommandOption command_option = parser.GetCommandOption();
                if (auto pList = std::get_if<ListOption>(&command_option)) {
                    const auto& list_option = *pList;
                    switch (list_option) {
                        case ListOption::PENDING:
                            manager.PrintTasks(false);
                            break;
                        case ListOption::COMPLETED:
                            manager.PrintTasks(true);
                            break;
                    }
                } else {
                    manager.PrintTasks();
                }
                break;
            }
            case TypeCommand::CLEAR:
                manager.ClearTasks();
                manager.Save();
                std::cout << "All tasks in "s << manager.GetFullName() << " cleared successfully\n";
                break;
            case TypeCommand::DONE: {
                const std::optional<size_t> index_opt = parser.GetTaskIndex();
                if (index_opt == std::nullopt) {
                    throw std::runtime_error("Task index was not specified"s);
                }

                const size_t index = index_opt.value();
                if (!manager.TaskExists(index)) {
                    const std::string message =
                        std::format("Task with index {} does not exist", index);
                    throw std::runtime_error(message);
                }

                manager.ToggleTask(index);
                manager.Save();
                std::cout << "Task status toggled successfully.\n";
                break;
            }
            case TypeCommand::REMOVE: {
                const std::optional<size_t> index_opt = parser.GetTaskIndex();
                if (index_opt == std::nullopt) {
                    throw std::runtime_error("Task index was not specified"s);
                }

                const size_t index = index_opt.value();
                if (!manager.TaskExists(index)) {
                    const std::string message =
                        std::format("Task with index {} does not exist", index);
                    throw std::runtime_error(message);
                }

                manager.RemoveTask(index);
                manager.Save();
                std::cout << std::format("Task with index {} removed successfully", index) << '\n';
                break;
            }
            case TypeCommand::EDIT: {
                const std::optional<size_t> index_opt = parser.GetTaskIndex();
                if (index_opt == std::nullopt) {
                    throw std::runtime_error("Task index was not specified"s);
                }

                const size_t index = index_opt.value();
                if (!manager.TaskExists(index)) {
                    const std::string message =
                        std::format("Task with index {} does not exist", index);
                    throw std::runtime_error(message);
                }

                manager.EditTask(index, parser.GetTaskText());
                manager.Save();
                std::cout << std::format("Task with index {} edited successfully", index) << '\n';
                break;
            }
            case TypeCommand::CONFIG: {
                const CommandOption command_option = parser.GetCommandOption();
                if (auto pConfig = std::get_if<ConfigOption>(&command_option)) {
                    const auto& config_option = *pConfig;
                    switch (config_option) {
                        case ConfigOption::PATH:
                            manager.SetPath(parser.GetTaskText(),
                                            DEFAULT_CONFIG_DIR + "/" + DEFAULT_CONFIG_NAME);
                            std::cout << "Path updated successfully.\n";
                            break;
                        case ConfigOption::NAME:
                            manager.SetName(parser.GetTaskText(),
                                            DEFAULT_CONFIG_DIR + "/" + DEFAULT_CONFIG_NAME);
                            std::cout << "Filename updated successfully.\n";
                            break;
                    }
                }
                break;
            }
            default:
                break;
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        PrintHelp();
    }

    return 0;
}
