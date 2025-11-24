#include "Message.hpp"

#include <iostream>
#include <stdexcept>

namespace bot {

Message::Message(const json& data) {
    if (!data.is_object()) {
        throw std::invalid_argument("Message data must be a JSON object");
    }
    
    if (data.contains("message_id")) {
        if (!data["message_id"].is_number()) {
            throw std::invalid_argument("message_id must be a number");
        }
        id_ = data["message_id"];
    }
    
    if (data.contains("chat") && data["chat"].contains("id")) {
        if (!data["chat"]["id"].is_number()) {
            throw std::invalid_argument("chat.id must be a number");
        }
        chat_id_ = data["chat"]["id"];
    }
    
    if (data.contains("text")) {
        if (!data["text"].is_string()) {
            throw std::invalid_argument("text must be a string");
        }
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
    
    size_t space_pos = text_.find(' ');
    if (space_pos != std::string::npos) {
        return text_.substr(1, space_pos - 1);  // Убираем '/' в начале
    } else {
        return text_.substr(1);  // Убираем '/' в начале
    }
}

}  // namespace bot