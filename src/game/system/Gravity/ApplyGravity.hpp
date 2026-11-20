#pragma once

#include "core/Core.hpp"

namespace game::system {
/// @brief Applies Doom's gravity to every character controller. Runs once per tic, in FixedTimeUpdate.
void ApplyGravity(Engine::Core &core);
} // namespace game::system
