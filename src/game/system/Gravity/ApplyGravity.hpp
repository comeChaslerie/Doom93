#pragma once

#include "core/Core.hpp"

namespace game::system {
/// @brief Applies Doom's gravity to every character controller that has a CapsuleCollider and a Floor: a body that
///   ended the last tic above its Floor falls. Runs once per tic, in FixedTimeUpdate, before ApplyStepUp, which still
///   holds the Floor of the last tic and lands the body.
void ApplyGravity(Engine::Core &core);
} // namespace game::system
