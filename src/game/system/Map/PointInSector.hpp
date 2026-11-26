#pragma once

#include "game/loader/LumpsData.hpp"
#include <cstddef>
#include <glm/vec2.hpp>

namespace game::system {
/// @brief Sector a subsector belongs to: the sector of the sidedef its first seg runs along (P_GroupLines).
const game::loader::Sector &GetSubsectorSector(const game::loader::Level &level,
                                               const game::loader::SubSector &subsector);

/// @brief Index of the subsector containing a map point, found by walking the BSP tree from its root, like
///   R_PointInSubsector. A point lying exactly on a partition line goes to the same child as in Doom.
/// @param level Level whose BSP tree is walked. A level without nodes holds a single subsector.
/// @param point Map coordinates (x, y), not world ones.
std::size_t PointInSubsector(const game::loader::Level &level, glm::vec2 point);

/// @brief Sector containing a map point (R_PointInSubsector, then the subsector's sector).
/// @param level Level whose BSP tree is walked.
/// @param point Map coordinates (x, y), not world ones.
const game::loader::Sector &PointInSector(const game::loader::Level &level, glm::vec2 point);
} // namespace game::system
