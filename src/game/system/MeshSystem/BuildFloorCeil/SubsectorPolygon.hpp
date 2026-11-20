#pragma once

#include "game/loader/LumpsData.hpp"
#include <glm/vec2.hpp>
#include <vector>

namespace game::system {
// Rebuilds the convex polygon of every subsector, indexed like level.subsectors.
// Segs alone cannot describe a subsector: edges created by BSP partition lines
// have no seg (freedoom E1M1: 152/682 subsectors own a single seg). Each leaf
// polygon is therefore the map bounding box clipped by its ancestor partition
// lines, then by its own seg lines.
// Polygons are wound clockwise (map coords, y up) like Doom seg loops; a
// degenerate subsector yields an empty polygon.
std::vector<std::vector<glm::vec2>> BuildSubsectorPolygons(const game::loader::Level &level);
} // namespace game::system
