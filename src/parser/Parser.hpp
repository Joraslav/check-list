#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace parser {

enum class TypeCommand { ADD, LIST, CLEAR, DONE, REMOVE, EDIT, HELP, CONFIG };

enum class ListOption { PENDING, COMPLETED };

enum class ConfigOption { PATH, NAME };

TypeCommand CommandToEnum(const std::string& type);

bool IsValidCommandWords(const TypeCommand& command, int count);

size_t WordToNumber(const std::string& word);

std::string ToLower(const std::string& str);

class Parser {
 public:
    using CommandOption = std::variant<std::monostate, ListOption, ConfigOption>;

    Parser() = default;

    const TypeCommand& GetTypeCommand() const { return command_.type; }

    const CommandOption& GetCommandOption() const { return command_.option; }

    const std::string& GetTaskText() const { return command_.text; }

    const std::optional<size_t>& GetTaskIndex() const { return command_.task_index; }

    void Parse(int& argc, char** argv);

 private:
    struct Command {
        TypeCommand type = TypeCommand::HELP;
        CommandOption option = std::monostate{};
        std::string text = "";
        std::optional<size_t> task_index = std::nullopt;
    } command_;
};

}  // namespace parser
