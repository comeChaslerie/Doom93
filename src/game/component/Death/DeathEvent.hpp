#pragma once

#include "entity/EntityId.hpp"

namespace game::component {
struct DeathEvent {
    Engine::EntityId entity;
};
} // namespace game::component
