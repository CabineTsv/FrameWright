#pragma once

#include "bot.hpp"

namespace macro_conversion {
struct LoadResult {
    BotReplay replay;
    bool legacyXD = false;
};

struct ImportResult {
    std::filesystem::path path;
    bool legacyXD = false;
};

LoadResult load(std::filesystem::path const& path);

BotReplay importData(std::vector<uint8_t>& data);

gdr::Result<std::vector<uint8_t>> exportData(BotReplay& replay, SaveFormat format);

int save(BotReplay& replay,
         std::string const& author,
         std::string const& description,
         std::filesystem::path const& path,
         SaveFormat format = SaveFormat::GDR2);

geode::Result<ImportResult> importFile(std::filesystem::path const& sourcePath,
                                       std::filesystem::path const& targetDirectory);
} // namespace macro_conversion
