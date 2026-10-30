#include "Config.hpp"

#include "Error.hpp"
#include "glaze/core/common.hpp"
#include "glaze/core/context.hpp"
#include "glaze/core/reflect.hpp"
#include "glaze/forward.hpp"
#include "glaze/json/read.hpp"
#include "Result.hpp"

#include <cstdlib>
#include <expected>
#include <filesystem>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace glz {

template <>
struct meta<tools::Config> {
    using T = tools::Config;
    // Glaze meta contract requires the member name `value`.
    // NOLINTNEXTLINE(readability-identifier-naming)
    [[maybe_unused]] static constexpr auto value = object(
        "storage_dir", &T::storage_dir, "storage_name", &T::storage_name);
};

}  // namespace glz

namespace fs = std::filesystem;
using namespace std::string_literals;
using tools::Config;
using tools::Error;
using tools::ErrorCode;
using tools::Result;

namespace {

[[nodiscard]]
Result<Config> ParseJson(const fs::path& config_path) {
    Config config;
    std::string buffer;
    if (const auto ec =
            glz::read_file_json(config, config_path.string(), buffer);
        ec) {
        if (ec.ec == glz::error_code::file_open_failure) {
            return std::unexpected(Error{
                .code = ErrorCode::NotFounded,
                .message = std::format("Cannot open config file: {}",
                                       config_path.string()),
            });
        }
        if (ec.ec == glz::error_code::parse_error) {
            return std::unexpected(Error{
                .code = ErrorCode::Parse,
                .message = std::format("Failed to parse config: {}",
                                       glz::format_error(ec, buffer)),
            });
        }
        return std::unexpected(Error{
            .code = ErrorCode::Unknown,
            .message = std::format("Unknown error parsing config: {}",
                                   glz::format_error(ec, buffer)),
        });
    }
    return config;
}

}  // namespace

namespace tools {

Result<fs::path> ConfigManager::GetDefaultConfigDir() {
    static const std::optional<fs::path> cached_path =
        [] -> std::optional<fs::path> {
        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        if (const char* const xdg_config_home = std::getenv("XDG_CONFIG_HOME");
            xdg_config_home != nullptr) {
            return fs::path{xdg_config_home};
        }
        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        if (const char* const home = std::getenv("HOME"); home != nullptr) {
            return fs::path{home} / ".config";
        }
        return std::nullopt;
    }();

    if (cached_path.has_value()) {
        return cached_path.value();
    }
    return std::unexpected(Error{
        .code = ErrorCode::NotFounded,
        .message = "Cannot find default config path"s,
    });
}

Result<Config> ConfigManager::LoadConfig(const fs::path& config_path) {
    if (!fs::exists(config_path)) {
        return std::unexpected(Error{
            .code = ErrorCode::NotFounded,
            .message = "Config path does not exists"s,
        });
    }
    return ParseJson(config_path);
}

ConfigManager::ConfigManager(const std::optional<fs::path>& config_path) {
    if (!config_path.has_value()) {
        const auto default_config_dir = GetDefaultConfigDir();
        if (!default_config_dir.has_value()) {
            throw std::runtime_error(
                std::format("{}", default_config_dir.error()));
        }
        const fs::path path_config =
            default_config_dir.value() / fs::path{config_name_};
        const auto config = LoadConfig(path_config);
        if (!config.has_value()) {
            throw std::runtime_error(std::format("{}", config.error()));
        }
        config_ = config.value();
    }
}

void ConfigManager::SetStorageDir(std::string_view new_storage_dir) noexcept {
    config_.storage_dir = fs::path{new_storage_dir};
}

void ConfigManager::SetStorageName(std::string_view new_storage_name) noexcept {
    config_.storage_name = new_storage_name;
}

fs::path ConfigManager::GetStoragePath() const {
    return config_.storage_dir / fs::path{config_.storage_name};
}

}  // namespace tools
