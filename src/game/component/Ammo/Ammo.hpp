#pragma once

#include "game/component/Ammo/AmmoType.hpp"
#include <unordered_map>

namespace game::component {
struct Ammo {
    std::unordered_map<AmmoType, int> stock;
};
} // namespace game::component
