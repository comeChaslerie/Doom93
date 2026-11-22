#pragma once

#include "core/Core.hpp"

namespace game::system {
/// @brief Keeps every character controller that has a CapsuleCollider and a Floor on Doom's floor, like the floor
///   checks of P_ZMovement. It finds the floor the character's move of this tic ends on (see ComputeFloorHeight) and
///   stores it in the Floor. It lifts the character onto that floor when it is below, which is how a body climbs a
///   step, and lands a falling character on it instead of letting it sink below. Runs once per tic, in
///   FixedTimeUpdate, after ApplyGravity and before the physics engine moves the character.
void ApplyStepUp(Engine::Core &core);
} // namespace game::system
