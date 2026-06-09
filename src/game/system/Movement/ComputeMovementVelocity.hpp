#pragma once

#include "game/component/Velocity/Velocity.hpp"
#include "game/ressources/Keys/Keys.hpp"

namespace game::system {
void ComputeMovementVelocity(component::Velocity &velocity, game::ressource::Keys &keys);
}
