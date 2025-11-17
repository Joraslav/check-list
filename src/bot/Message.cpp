#include "Message.hpp"

#include <iostream>

namespace bot {

Message::Message(const json& data) {
    if (data.contains("message_id")) {
        id_ = data["message_id"];
    }
    
    if (data.contains("chat") && data["chat"].contains("id")) {
        chat_id_ = data["chat"]["id"];
    }
    
    if (data.contains("text")) {
        text_ = data["text"];
        is_command_ = !text_.empty() && text_[0] == '/';
    }
}

bool Message::IsCommand() const {
    return is_command_;
}

std::string Message::GetCommand() const {
    if (!is_command_) {
        return "";
    }
    
    // Извлекаем команду из текста (до первого пробела)
    size_t space_pos = text_.find(' ');
    if (space_pos != std::string::npos) {
        return text_.substr(1, space_pos - 1);  // Убираем '/' в начале
    } else {
        return text_.substr(1);  // Убираем '/' в начале
    }
}

}  // namespace bot