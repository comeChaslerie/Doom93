#pragma once

#include "game/component/EnemyState/EnemyState.hpp"

namespace game::system {
struct Perception {
    bool isVisible = false;
    bool isReachable = false;
    bool isTouch = false;
    bool isDead = false;
    bool isAttackFinish = false;
    bool isPainFinish = false;
};
[[nodiscard]] component::EnemyState GetNextState(const component::EnemyState &state, const Perception &perception);
} // namespace game::system
