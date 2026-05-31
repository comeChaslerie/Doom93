#pragma once

#include "game/component/Stat/Stat.hpp"

namespace game::component {
struct Health {
    Stat<float> hp{100.f};
};
} // namespace game::component
