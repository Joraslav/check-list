#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace bot {

class Message {
 public:
    using json = nlohmann::json;
    
    Message() = default;
    explicit Message(const json& data);
    
    long GetId() const { return id_; }
    long GetChatId() const { return chat_id_; }
    const std::string& GetText() const { return text_; }
    bool IsCommand() const;
    std::string GetCommand() const;
    
 private:
    long id_ = 0;
    long chat_id_ = 0;
    std::string text_;
    bool is_command_ = false;
};

}  // namespace bot