#include "Parser.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <format>
#include <locale>
#include <ranges>
#include <stdexcept>
#include <unordered_map>

namespace parser {

// TODO: re-write on magic_enum
TypeCommand CommandToEnum(const std::string& type) {
    std::unordered_map<std::string, TypeCommand> commands = {
        {"add", TypeCommand::ADD},   {"list", TypeCommand::LIST},     {"clear", TypeCommand::CLEAR},
        {"done", TypeCommand::DONE}, {"remove", TypeCommand::REMOVE}, {"edit", TypeCommand::EDIT},
        {"help", TypeCommand::HELP}, {"config", TypeCommand::CONFIG}};

    if (commands.contains(type)) {
        return commands[type];
    } else {
        throw std::invalid_argument(std::format("Unknown command: {}", type));
    }
}

bool IsValidCommandWords(const TypeCommand& command, int count) {
    switch (command) {
        case TypeCommand::ADD:
        case TypeCommand::DONE:
        case TypeCommand::REMOVE:
            return count != 3 ? false : true;
            break;
        case TypeCommand::EDIT:
            return count < 4 ? false : true;
            break;
        case TypeCommand::LIST:
            return (count > 1 && count < 4) ? false : true;
            break;
        case TypeCommand::CONFIG:
            return count < 4 ? false : true;
            break;
        default:
            return false;
            break;
    }
}

int WordToNumber(const std::string& word) {
    if (word.empty() || !std::all_of(word.begin(), word.end(), ::isdigit)) {
        throw std::invalid_argument(std::format("Word ({}) contains non-digit characters", word));
    }

    int number;
    auto result = std::from_chars(word.data(), word.data() + word.size(), number);

    if (result.ec != std::errc()) {
        throw std::invalid_argument(std::format("Failed to convert word ({}) to number", word));
    }

    return number;
}

std::string ToLower(const std::string& str) {
    auto to_lower = [](char c) { return std::tolower(c); };

    std::string lower;
    lower.reserve(str.size());

    std::transform(str.begin(), str.end(), std::back_inserter(lower), to_lower);

    return lower;
}

void Parser::Parse(const int& argc, const char** argv) {
    if (argc < 2) {
        throw std::invalid_argument("No command entered");
    }

    std::string type_str = ToLower(std::string(argv[1]));
    TypeCommand type = CommandToEnum(type_str);
    if (!IsValidCommandWords(type, argc)) {
        throw std::invalid_argument("");
    }

    switch (type) {
        case TypeCommand::ADD: {
            command_.type = type;
            for (int i = 2; i < argc; ++i) {
                std::string tmp_str = ToLower(std::string(argv[i]));
                command_.text += " " + tmp_str;
            }
            break;
        }
        case TypeCommand::LIST: {
            command_.type = type;
            if (argc == 3) {
                std::string option_list = ToLower(std::string(argv[2]));
                if (option_list == "pending") {
                    command_.option = ListOption::PENDING;
                } else if (option_list == "completed") {
                    command_.option = ListOption::COMPLETED;
                } else {
                    throw std::invalid_argument("Unknown list options");
                }
            }
            break;
        }
        case TypeCommand::CLEAR: {
            command_.type = type;
            break;
        }
        case TypeCommand::DONE: {
            command_.type = type;

            int number = WordToNumber(argv[2]);
            if (number < 0) {
                command_.task_index = std::nullopt;
            } else {
                command_.task_index = number;
            }
            break;
        }
        case TypeCommand::REMOVE: {
            command_.type = type;

            int number = WordToNumber(argv[2]);
            if (number < 0) {
                command_.task_index = std::nullopt;
            } else {
                command_.task_index = number;
            }
            break;
        }
        case TypeCommand::EDIT: {
            int number = WordToNumber(argv[2]);
            if (number < 0) {
                command_.task_index = std::nullopt;
            } else {
                command_.task_index = number;
            }

            for (int i = 3; i < argc; ++i) {
                std::string tmp_str = ToLower(std::string(argv[i]));
                command_.text += " " + tmp_str;
            }
            break;
        }
        case TypeCommand::HELP: {
            command_.type = type;
            break;
        }
        case TypeCommand::CONFIG: {
            command_.type = type;
            std::string option_config = ToLower(std::string(argv[2]));
            if (option_config == "path") {
                command_.option = ConfigOption::PATH;
            } else if (option_config == "name") {
                command_.option = ConfigOption::NAME;
            } else {
                throw std::invalid_argument("Unknown config options");
            }

            for (int i = 3; i < argc; ++i) {
                std::string tmp_str = ToLower(std::string(argv[i]));
                command_.text += " " + tmp_str;
            }

            break;
        }
        default: {
            throw std::invalid_argument("Unknown command");
            break;
        }
    }
}
}  // namespace parser
