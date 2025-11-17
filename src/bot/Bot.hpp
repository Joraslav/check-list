#pragma once

#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

#include <string>
#include <functional>

namespace bot {

class Bot {
 public:
    using json = nlohmann::json;

    Bot(const std::string& token);

    void Start();
    void Stop();
    
 private:
    std::string token_;
    std::string api_url_;

    json MakeRequest(const std::string& method, const json& data);
    json GetUpdates(long offset);
    void SendMessage(long chat_id, const std::string& text);
    
};

}  // namespace bot
