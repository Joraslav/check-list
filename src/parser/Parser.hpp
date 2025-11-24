#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace parser {
// Types of commands
enum class TypeCommand { ADD, LIST, CLEAR, DONE, REMOVE, EDIT, HELP, CONFIG };

// Options of the list command
enum class ListOption { PENDING, COMPLETED };

enum class ConfigOption { PATH, NAME };

// Options depending on the command
using CommandOption = std::variant<std::monostate, ListOption, ConfigOption>;

// Converting a string to lowercase
std::string ConvertToLower(const std::string& str);

class Parser {
 public:
    Parser() = default;

    // Get the command type
    const TypeCommand GetType() const { return command.type; }

    // Get the command option
    const CommandOption GetOption() const { return command.option; }

    // Get the command text
    const std::string GetTaskText() const { return command.text_command; }

    // Get the command index
    const std::optional<size_t> GetTaskIndex() const { return command.task_index; }

    // Parsing the entered command
    void Parse(const int& argc, const char** argv);

 private:
    struct Command {
        // Type command
        TypeCommand type = TypeCommand::HELP;
        // Command option
        CommandOption option = std::monostate{};
        // Description of the command
        std::string text_command = "";
        // The command index
        std::optional<size_t> task_index;
    };
    Command command;
};

}  // namespace parser
