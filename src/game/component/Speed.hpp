#pragma once

#include "game/component/Stat/Stat.hpp"

namespace game::component {
struct Speed {
    Stat<float> speed{250.f};
};
} // namespace game::component
