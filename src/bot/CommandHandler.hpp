#pragma once

#include "Message.hpp"
#include "../task/Task.hpp"

#include <nlohmann/json.hpp>
#include <functional>
#include <string>
#include <unordered_map>

namespace bot {

class CommandHandler {
 public:
    using json = nlohmann::json;
    
    CommandHandler(task::TaskManager& task_manager);
    
    void HandleCommand(const Message& message, std::function<void(const std::string&, long)> send_message_callback);
    
 private:
    task::TaskManager& task_manager_;
    
    // Обработчики команд
    void HandleStart(const Message& message, std::function<void(const std::string&, long)> send_message_callback);
    void HandleHelp(const Message& message, std::function<void(const std::string&, long)> send_message_callback);
    void HandleAdd(const Message& message, std::function<void(const std::string&, long)> send_message_callback);
    void HandleList(const Message& message, std::function<void(const std::string&, long)> send_message_callback);
    void HandleDone(const Message& message, std::function<void(const std::string&, long)> send_message_callback);
    void HandleRemove(const Message& message, std::function<void(const std::string&, long)> send_message_callback);
    void HandleClear(const Message& message, std::function<void(const std::string&, long)> send_message_callback);
    
    // Вспомогательные методы
    std::string GetTaskListString() const;
    std::string ExtractCommandArgument(const std::string& text, const std::string& command) const;
};

}  // namespace bot