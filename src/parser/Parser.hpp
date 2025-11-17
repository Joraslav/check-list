#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace parse {

class Parser 
{
   public:
      Parser() = default;

      const std::string GetType() const { return command.type_; }
      
      const std::string GetTaskText() const { return command.task_text_; }

      const int GetId() const { return command.id_;}

      void Parse(const int& argc, const char** argv);
      
      //const std::string& GetAllTaskText(int argc, std::string_view argv) const;
   private:
      struct Command
      {
         //enum Type {ADD, DELETE, LIST, ANOTHER} type_ = ANOTHER;
         std::string type_ = "";
         std::string task_text_ = "";
         int id_ = -1;
      };
      Command command;
};

}  // namespace parse
