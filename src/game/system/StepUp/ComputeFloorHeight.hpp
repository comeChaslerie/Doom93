#pragma once

#include "game/loader/LumpsData.hpp"
#include <glm/vec2.hpp>

namespace game::system {
/// @brief Highest step a body climbs by walking into it, in map units (24 * FRACUNIT in P_TryMove).
inline constexpr float MAX_STEP_HEIGHT = 24.f;

/// @brief What P_TryMove needs to know about a body.
struct Body {
    /// @brief Center of the body, in map coordinates (x, y).
    glm::vec2 position{0.f, 0.f};
    /// @brief Height of the bottom of the body, in map units (mobj->z).
    float feet = 0.f;
    /// @brief Half the side of the square box Doom gives the body (mobj->radius).
    float radius = 0.f;
    /// @brief Height of the body (mobj->height).
    float height = 0.f;
};

/// @brief Height of the floor under a body once it has made this tic's move, following P_TryMove: the floor of where
///   the move ends, or of where the body stands if the move fails (mobj->floorz).
///
///   The floor is the highest one under the body's square box: the sector of its center, and both sides of every
///   two-sided linedef crossing the box. A body thus stays on a ledge as long as part of its box is over it.
///
///   A move that reaches a floor at most MAX_STEP_HEIGHT above the feet, with room for the body above it, returns that
///   floor even when it is above the feet: the body has to be lifted onto it at once. Since Doom compares the step with
///   the feet and not with the old floor, a falling body catches a ledge the same way.
///
///   Floors higher than that count as walls. Walls are left to the physics engine: a move that runs into one is tried
///   again along it, the way the character slides, so that a body hugging a wall still climbs the stairs beside it.
/// @param level Level the body moves in.
/// @param body The body, before the move.
/// @param move Horizontal move of this tic, in map units.
/// @return The floor height, in map units. It is above body.feet when the body has to step up.
float ComputeFloorHeight(const game::loader::Level &level, const Body &body, glm::vec2 move);
} // namespace game::system
