#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace parse {

class Parser {
 public:
    Parser(const std::string& command) : command_(command) {};

    const std::string& GetAllTaskText(int argc, std::string_view argv) const;

 private:
    std::string command_;
    std::string task_text_;
    std::string filter_;
};

}  // namespace parse
