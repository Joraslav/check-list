#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace parser {

enum class TypeCommand { ADD, LIST, CLEAR, DONE, REMOVE, EDIT, HELP, CONFIG };

enum class ListOption { PENDING, COMPLETED };

enum class ConfigOption { PATH, NAME };

using CommandOption = std::variant<std::monostate, ListOption, ConfigOption>;

TypeCommand CommandToEnum(const std::string& type);

bool IsValidCommandWords(const TypeCommand& command, int count);

int WordToNumber(const std::string& word);

std::string ToLower(const std::string& str);

class Parser {
 public:
    Parser() = default;

    TypeCommand GetTypeCommand() const { return command_.type; }

    CommandOption GetCommandOption() const { return command_.option; }

    std::string GetTaskText() const { return command_.text; }

    std::optional<size_t> GetTaskIndex() const { return command_.task_index; }

    void Parse(const int& argc, const char** argv);

 private:
    struct Command {
        TypeCommand type = TypeCommand::HELP;
        CommandOption option = std::monostate{};
        std::string text = "";
        std::optional<size_t> task_index;
    } command_;
};

}  // namespace parser
