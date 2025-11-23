// The file describes a Parser class that stores the type, option, text, and
// index of a command according to the selected command. Some do not work with
// indexes, options, and indexes, so the default Command structure fields are
// defined for them.
// DEV_MODE is used to configure paths and directories for application data

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <optional>

namespace parser 
{
   // Types of commands
   enum class TypeCommand { ADD, LIST, CLEAR, DONE, REMOVE, EDIT, HELP
      #ifdef DEV_MODE
      , CONFIG
      #endif
   };

   // Options of the list command
   enum class ListOption { PENDING, COMPLETED };

   #ifdef DEV_MODE
   // Options of the config command
   enum class ConfigOption { PATH, NAME };
   #endif

   // Options depending on the command
   using CommandOption = std::variant<std::monostate, ListOption
   #ifdef DEV_MODE
   , ConfigOption
   #endif
   >;

   // Converting a string to lowercase
   std::string ConvertToLower(const std::string& str);

   class Parser 
   {
      public:
         Parser() = default;

         // Get the command type
         const TypeCommand GetType() const { return command.type; }

         // Get the command option
         const CommandOption GetOption() const { return command.option; }
         
         // Get the command text
         const std::string GetTaskText() const { return command.text_command; }

         // Get the command index 
         const std::optional<size_t> GetTaskIndex() const { return command.task_index;}

         // Parsing the entered command
         void Parse(const int& argc, const char** argv);

      private:
         struct Command
         {
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
