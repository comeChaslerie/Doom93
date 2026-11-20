#pragma once

namespace game::system {
/// @brief Doom gravity, in map units per tic per tic (GRAVITY = FRACUNIT in p_local.h).
inline constexpr float GRAVITY = 1.f;
/// @brief Vertical speed of a body on its first tic off the floor, in map units per tic (momz = -GRAVITY*2 in
///   P_ZMovement). Negative, since the body goes down.
inline constexpr float FALL_START_SPEED = -2.f * GRAVITY;

/// @brief Vertical speed for the next tic, following Doom's P_ZMovement: a body that leaves the floor starts falling
///   at FALL_START_SPEED, then gains one gravity per tic, with no terminal velocity.
/// @param verticalSpeed Current vertical speed, in map units per tic (negative when falling).
/// @param onFloor Whether the body stands on the floor.
/// @return The new vertical speed, in map units per tic.
float ComputeVerticalSpeed(float verticalSpeed, bool onFloor);
} // namespace game::system
