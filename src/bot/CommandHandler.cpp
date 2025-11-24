#include "CommandHandler.hpp"

#include <iostream>
#include <sstream>

namespace bot {

CommandHandler::CommandHandler(task::TaskManager& task_manager) : task_manager_(task_manager) {}

void CommandHandler::HandleCommand(const Message& message, std::function<void(const std::string&, long)> send_message_callback) {
    std::string command = message.GetCommand();
    
    if (command == "start") {
        HandleStart(message, send_message_callback);
    } else if (command == "help") {
        HandleHelp(message, send_message_callback);
    } else if (command == "add") {
        HandleAdd(message, send_message_callback);
    } else if (command == "list") {
        HandleList(message, send_message_callback);
    } else if (command == "done") {
        HandleDone(message, send_message_callback);
    } else if (command == "remove") {
        HandleRemove(message, send_message_callback);
    } else if (command == "clear") {
        HandleClear(message, send_message_callback);
    } else {
        // Неизвестная команда
        std::string response = "Неизвестная команда. Используйте /help для получения списка доступных команд.";
        send_message_callback(response, message.GetChatId());
    }
}

void CommandHandler::HandleStart(const Message& message, std::function<void(const std::string&, long)> send_message_callback) {
    std::string response = "Добро пожаловать в Todo List бот!\n"
                          "Я помогу вам управлять вашими задачами.\n"
                          "Используйте /help для получения списка доступных команд.";
    send_message_callback(response, message.GetChatId());
}

void CommandHandler::HandleHelp(const Message& message, std::function<void(const std::string&, long)> send_message_callback) {
    std::string response = "Доступные команды:\n"
                          "/start - Начать работу с ботом\n"
                          "/help - Показать это сообщение\n"
                          "/add <текст задачи> - Добавить новую задачу\n"
                          "/list - Показать все задачи\n"
                          "/done <номер задачи> - Отметить задачу как выполненную/невыполненную\n"
                          "/remove <номер задачи> - Удалить задачу\n"
                          "/clear - Очистить все задачи";
    send_message_callback(response, message.GetChatId());
}

void CommandHandler::HandleAdd(const Message& message, std::function<void(const std::string&, long)> send_message_callback) {
    std::string task_text = ExtractCommandArgument(message.GetText(), "add");
    
    if (task_text.empty()) {
        send_message_callback("Пожалуйста, укажите текст задачи. Пример: /add Купить молоко", message.GetChatId());
        return;
    }
    
    // Проверяем длину текста задачи
    if (task_text.length() > 1000) {
        send_message_callback("Текст задачи слишком длинный (максимум 1000 символов)", message.GetChatId());
        return;
    }
    
    try {
        task_manager_.AddTask(task_text);
        task_manager_.Save();
        send_message_callback("Задача добавлена успешно!", message.GetChatId());
    } catch (const std::exception& e) {
        send_message_callback("Ошибка при добавлении задачи: " + std::string(e.what()), message.GetChatId());
    }
}

void CommandHandler::HandleList(const Message& message, std::function<void(const std::string&, long)> send_message_callback) {
    try {
        std::string task_list = GetTaskListString();
        if (task_list.empty()) {
            send_message_callback("Список задач пуст.", message.GetChatId());
        } else {
            send_message_callback(task_list, message.GetChatId());
        }
    } catch (const std::exception& e) {
        send_message_callback("Ошибка при получении списка задач: " + std::string(e.what()), message.GetChatId());
    }
}

void CommandHandler::HandleDone(const Message& message, std::function<void(const std::string&, long)> send_message_callback) {
    std::string index_str = ExtractCommandArgument(message.GetText(), "done");
    
    if (index_str.empty()) {
        send_message_callback("Пожалуйста, укажите номер задачи. Пример: /done 0", message.GetChatId());
        return;
    }
    
    try {
        // Проверяем, что строка содержит только цифры
        if (index_str.find_first_not_of("0123456789") != std::string::npos) {
            send_message_callback("Неверный формат номера задачи. Пожалуйста, укажите число.", message.GetChatId());
            return;
        }
        
        size_t index = std::stoull(index_str);
        
        if (!task_manager_.TaskExists(index)) {
            send_message_callback("Задача с номером " + std::to_string(index) + " не существует.", message.GetChatId());
            return;
        }
        
        task_manager_.ToggleTask(index);
        task_manager_.Save();
        send_message_callback("Статус задачи обновлен.", message.GetChatId());
    } catch (const std::invalid_argument&) {
        send_message_callback("Неверный формат номера задачи. Пожалуйста, укажите число.", message.GetChatId());
    } catch (const std::exception& e) {
        send_message_callback("Ошибка при обновлении статуса задачи: " + std::string(e.what()), message.GetChatId());
    }
}

void CommandHandler::HandleRemove(const Message& message, std::function<void(const std::string&, long)> send_message_callback) {
    std::string index_str = ExtractCommandArgument(message.GetText(), "remove");
    
    if (index_str.empty()) {
        send_message_callback("Пожалуйста, укажите номер задачи. Пример: /remove 0", message.GetChatId());
        return;
    }
    
    try {
        // Проверяем, что строка содержит только цифры
        if (index_str.find_first_not_of("0123456789") != std::string::npos) {
            send_message_callback("Неверный формат номера задачи. Пожалуйста, укажите число.", message.GetChatId());
            return;
        }
        
        size_t index = std::stoull(index_str);
        
        if (!task_manager_.TaskExists(index)) {
            send_message_callback("Задача с номером " + std::to_string(index) + " не существует.", message.GetChatId());
            return;
        }
        
        task_manager_.RemoveTask(index);
        task_manager_.Save();
        send_message_callback("Задача удалена.", message.GetChatId());
    } catch (const std::invalid_argument&) {
        send_message_callback("Неверный формат номера задачи. Пожалуйста, укажите число.", message.GetChatId());
    } catch (const std::exception& e) {
        send_message_callback("Ошибка при удалении задачи: " + std::string(e.what()), message.GetChatId());
    }
}

void CommandHandler::HandleClear(const Message& message, std::function<void(const std::string&, long)> send_message_callback) {
    try {
        task_manager_.ClearTasks();
        task_manager_.Save();
        send_message_callback("Все задачи удалены.", message.GetChatId());
    } catch (const std::exception& e) {
        send_message_callback("Ошибка при очистке задач: " + std::string(e.what()), message.GetChatId());
    }
}

std::string CommandHandler::GetTaskListString() const {
    const auto& tasks = task_manager_.GetTasks();
    
    if (tasks.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "Список задач:\n";
    
    for (size_t i = 0; i < tasks.size(); ++i) {
        oss << i << ". ";
        if (tasks[i].done) {
            oss << "[x] ";
        } else {
            oss << "[ ] ";
        }
        oss << tasks[i].text << "\n";
    }
    
    return oss.str();
}

std::string CommandHandler::ExtractCommandArgument(const std::string& text, const std::string& command) const {
    std::string prefix = "/" + command + " ";
    size_t pos = text.find(prefix);
    
    if (pos != std::string::npos) {
        return text.substr(pos + prefix.length());
    }
    
    return "";
}

}  // namespace bot