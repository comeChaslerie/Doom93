#pragma once

#include "game/loader/LumpsData.hpp"
#include <string>

namespace game::loader {
LumpData WadLoader(const std::string &filepath);
} // namespace game::loader
