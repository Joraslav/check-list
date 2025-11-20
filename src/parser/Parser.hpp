#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace parser 
{
   class Parser 
   {
      public:
         Parser() = default;

         const std::string GetType() const { return command.type_; }
         
         const std::string GetTaskText() const { return command.task_text_; }

         const int GetId() const { return command.id_;}

         void Parse(const int& argc, const char** argv);
      private:
         struct Command
         {
            std::string type_ = "";
            std::string task_text_ = "";
            int id_ = -1;
         };
         Command command;
};

}  // namespace parser
