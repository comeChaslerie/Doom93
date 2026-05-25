#pragma once

#include "game/component/DamageType.hpp"

namespace game::component
{
    struct DamageEvent {
        float amount;
        DamageType type;
    };
}
