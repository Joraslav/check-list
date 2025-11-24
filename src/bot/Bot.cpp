#include "Bot.hpp"

#include "CommandHandler.hpp"
#include "Message.hpp"
#include "Task.hpp"

#ifdef WIN32
#include <windows.h>
#endif

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

namespace bot {

namespace beast = boost::beast;    // from <boost/beast.hpp>
namespace http = beast::http;      // from <boost/beast/http.hpp>
namespace net = boost::asio;       // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;  // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;          // from <boost/asio/ip/tcp.hpp>

Bot::Bot(const std::string& token) : token_(token) {
    api_url_ = "https://api.telegram.org/bot" + token_;
}

void Bot::Start() {
    std::cout << "Starting Telegram bot...\n";

    // Создаем TaskManager
    task::TaskManager task_manager;

    // Создаем CommandHandler
    CommandHandler command_handler(task_manager);

    long offset = 0;

    while (true) {
        try {
            // Получаем обновления
            Bot::json updates = GetUpdates(offset);

            // Обрабатываем обновления
            for (const auto& update : updates["result"]) {
                if (update.contains("message")) {
                    Message message(update["message"]);
                    offset = std::max(offset, static_cast<long>(update["update_id"]) + 1);

                    // Обрабатываем команды
                    if (message.IsCommand()) {
                        command_handler.HandleCommand(
                            message, [this](const std::string& text, long chat_id) {
                                this->SendMessage(chat_id, text);
                            });
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } catch (const std::exception& e) {
            std::cerr << "Error in bot loop: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
}

void Bot::Stop() { std::cout << "Stoping Telegram bot..." << std::endl; }

Bot::json Bot::MakeRequest(const std::string& method, const Bot::json& data) {
    std::string host = "api.telegram.org";
    std::string port = "443";
    std::string target = "/bot" + token_ + "/" + method;

    net::io_context ioc;

    ssl::context ssl_ctx(ssl::context::tlsv12_client);
    ssl_ctx.set_default_verify_paths();

    tcp::resolver resolver(ioc);
    beast::ssl_stream<beast::tcp_stream> stream(ioc, ssl_ctx);

    if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
        beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        throw beast::system_error{ec};
    }

    const auto results = resolver.resolve(host, port);
    beast::get_lowest_layer(stream).connect(results);

    stream.handshake(ssl::stream_base::client);

    http::request<http::string_body> req{http::verb::post, target, 11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::content_type, "application/json");

    req.body() = data.dump();
    req.prepare_payload();

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::dynamic_body> res;
    http::read(stream, buffer, res);

    beast::error_code ec;
    stream.shutdown(ec);
    if (ec == net::error::eof) {
        ec = {};
    }

    std::string response_body = boost::beast::buffers_to_string(res.body().data());

    if (res.result() != http::status::ok) {
        throw std::runtime_error("HTTP request failed with status: " +
                                 std::to_string(res.result_int()) + ", body: " + response_body);
    }

    try {
        Bot::json response_json = Bot::json::parse(response_body);

        if (response_json.contains("ok") && response_json["ok"].is_boolean()) {
            if (!response_json["ok"]) {
                // Если "ok" равно false, выбрасываем исключение с сообщением об ошибке
                std::string error_message = "Telegram API error";
                if (response_json.contains("description") &&
                    response_json["description"].is_string()) {
                    error_message = response_json["description"];
                }
                throw std::runtime_error("Telegram API error: " + error_message);
            }
        } else {
            throw std::runtime_error(
                "Invalid Telegram API response: missing or invalid 'ok' field");
        }

        return response_json;
    } catch (const Bot::json::parse_error& e) {
        throw std::runtime_error("Failed to parse JSON response: " + std::string(e.what()) +
                                 ", body: " + response_body);
    }
}

Bot::json Bot::GetUpdates(long offset) {
    Bot::json data;
    data["offset"] = offset;
    data["timeout"] = 30;  // Таймаут в секундах
    return MakeRequest("getUpdates", data);
}

void Bot::SendMessage(long chat_id, const std::string& text) {
    Bot::json data;
    data["chat_id"] = chat_id;
    data["text"] = text;
    MakeRequest("sendMessage", data);
}

}  // namespace bot
