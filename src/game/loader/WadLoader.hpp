#pragma once

#include "game/loader/WADReader.hpp"
#include <string>

namespace game::loader {
WadInfo WadLoader(std::string &filepath);
} // namespace game::loader
