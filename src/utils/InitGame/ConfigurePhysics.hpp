#pragma once

#include "core/Core.hpp"

namespace utils::InitGame {
/// @brief Runs the fixed update, and so the physics, at Doom's tic rate. Must run after the physics Startup systems.
void ConfigurePhysics(Engine::Core &core);
} // namespace utils::InitGame
