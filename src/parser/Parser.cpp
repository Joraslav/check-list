#include "Parser.hpp"

#include <cctype>
#include <charconv>
#include <stdexcept>
#include <unordered_map>

namespace parser {

// Converting a command string to an enum type
static TypeCommand FormatInEnum(const std::string& type) {
    std::unordered_map<std::string, TypeCommand> u_map = {
        {"add", TypeCommand::ADD},       {"list", TypeCommand::LIST},
        {"clear", TypeCommand::CLEAR},   {"done", TypeCommand::DONE},
        {"remove", TypeCommand::REMOVE}, {"edit", TypeCommand::EDIT},
        {"help", TypeCommand::HELP},
        #ifdef DEV_MODE
        {"config", TypeCommand::CONFIG}
        #endif
    };

    auto it = u_map.find(type);
    if (it != u_map.end()) {
        return it->second;
    } else {
        throw std::invalid_argument("Unknown command");
    }
}

// Checking for the correctness of the number of arguments
static void ValidCommandWords(const TypeCommand& check_type, int count) {
    const std::string message = "Invalid number of command arguments";
    switch (check_type) {
        case TypeCommand::ADD:
        case TypeCommand::DONE:
        case TypeCommand::REMOVE: {
            if (count != 3) {
                throw std::invalid_argument(message);
            }
            break;
        }
        case TypeCommand::EDIT: {
            if (count < 4) {
                throw std::invalid_argument(message);
            }
        }
        case TypeCommand::LIST: {
            if (count > 3) {
                throw std::invalid_argument(message);
            }
        }
        #ifdef DEV_MODE
        case TypeCommand::CONFIG: {
            if (count < 4) {
                throw std::invalid_argument(message);
            }
        }
        #endif
        default: {
            throw std::invalid_argument("Unknown command");
            break;
        }
    }
}

// Converting a string to a number
static int ConvertWordInNumber(const std::string& str) {
    int number = 0;
    const char* first = str.data();
    const char* last = first + str.size();
    while (first != last && (*first == ' ' || *first == '\t')) {
        ++first;
    }

    auto res = std::from_chars(str.data(), str.data() + str.size(), number);
    if (res.ec != std::errc() || res.ptr != str.data() + str.size()) {
        throw std::invalid_argument("Extra chars");
    }
    return number;
}

// Converting a string to lowercase
std::string ConvertToLower(const std::string& str) {
    std::string convert_str = str;
    for (char& c : convert_str) {
        c = std::tolower(c);
    }
    return convert_str;
}

// Parsing the entered command
void Parser::Parse(const int& argc, const char** argv) {
    if (argc < 2) {
        throw std::invalid_argument("No command entered");
    }

    std::string type_str = ConvertToLower(std::string(argv[1]));
    TypeCommand type = FormatInEnum(type_str);
    ValidCommandWords(type, argc);
    switch (type) {
        case TypeCommand::ADD: {
            command.type = type;
            for (int i = 2; i < argc; ++i) {
                std::string tmp_str = ConvertToLower(std::string(argv[i]));
                command.text_command += " " + tmp_str;
            }
            break;
        }
        case TypeCommand::LIST: {
            command.type = type;
            if (argc == 3) {
                std::string option_list = ConvertToLower(std::string(argv[2]));
                if (option_list == "pending") { 
                    command.option = ListOption::PENDING;}
                else if (option_list == "completed") {
                    command.option = ListOption::COMPLETED;
                }
                else{
                    throw std::invalid_argument("Unknown list options");
                }
            }
            break;
        }
        case TypeCommand::CLEAR: {
            command.type = type;
            break;
        }
        case TypeCommand::DONE: {
            command.type = type;
            command.id = ConvertWordInNumber(argv[2]);
            break;
        }
        case TypeCommand::REMOVE: {
            command.type = type;
            command.id = ConvertWordInNumber(argv[2]);
            break;
        }
        case TypeCommand::EDIT: {
            command.id = ConvertWordInNumber(argv[2]);
            for (int i = 3; i < argc; ++i) {
                std::string tmp_str = ConvertToLower(std::string(argv[i]));
                command.text_command += " " + tmp_str;
            }
            break;
        }
        case TypeCommand::HELP: {
            command.type = type;
            break;
        }
        #ifdef DEV_MODE
        case TypeCommand::CONFIG: {
            command.type = type;
            std::string option_config = ConvertToLower(std::string(argv[2]));
            if (option_config == "path") { 
                command.option = ConfigOption::PATH;
            }
            else if (option_config == "name") {
                command.option = ConfigOption::NAME;
            }
            else{
                throw std::invalid_argument("Unknown config options");
            }

            for (int i = 3; i < argc; ++i) {
                std::string tmp_str = ConvertToLower(std::string(argv[i]));
                command.text_command += " " + tmp_str;
            }

            break;
        }
        #endif
        default: {
            throw std::invalid_argument("Unknown command");
            break;
        }
    }
}
}  // namespace parser
