#pragma once

#include "entity/EntityId.hpp"
#include "game/component/EnemyState/EnemyState.hpp"
#include <optional>

namespace game::component {
struct AIState {
    EnemyState state = EnemyState::Idle;
    std::optional<Engine::EntityId> target = std::nullopt;
};
} // namespace game::component
