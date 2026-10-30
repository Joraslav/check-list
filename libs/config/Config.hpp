#pragma once

#include "Result.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace tools {

struct Config final {
    std::filesystem::path storage_dir;
    std::string storage_name;
};

class ConfigManager final {
 public:
    explicit ConfigManager(
        const std::optional<std::filesystem::path>& config_path = std::nullopt);

    [[nodiscard]]
    static Result<std::filesystem::path> GetDefaultConfigDir();

    void SetStorageDir(std::string_view new_storage_dir) noexcept;
    void SetStorageName(std::string_view new_storage_name) noexcept;

    [[nodiscard]]
    std::filesystem::path GetStoragePath() const;

 private:
    [[nodiscard]]
    static tools::Result<Config> LoadConfig(
        const std::filesystem::path& config_path);

    Config config_;
    std::string config_name_ = "config.json";
};

}  // namespace tools
