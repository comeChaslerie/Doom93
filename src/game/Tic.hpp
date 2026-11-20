#pragma once

namespace game::tic {
/// @brief Doom runs its game logic at 35 tics per second (TICRATE).
inline constexpr float RATE = 35.f;
/// @brief Duration of one tic, in seconds.
inline constexpr float DURATION = 1.f / RATE;
} // namespace game::tic
