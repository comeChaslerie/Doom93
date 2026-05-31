#pragma once

#include "game/component/Stat/Stat.hpp"

namespace game::component {
struct Armor {
    Stat<float> armor{0.f};
};
} // namespace game::component
