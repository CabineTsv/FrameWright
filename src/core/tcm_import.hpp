#pragma once

#include "bot.hpp"

namespace tcm_import {
geode::Result<BotReplay> importTCM(std::vector<uint8_t> const& data);
}
