#include "game/system/Map/PointInSector.hpp"
#include <cstdint>

namespace {
/// @brief Set on a node child that is a subsector rather than another node (NF_SUBSECTOR).
constexpr std::uint32_t SUBSECTOR_FLAG = 0x8000;

/// @brief Side of a point relative to a partition line, like R_PointOnSide: false for the front (right) side, true
///   for the back (left) one. Computed in double, so it stays exact at map scale where float would round.
bool IsOnBackSide(glm::vec2 point, const game::loader::Node &node)
{
    const double x = point.x;
    const double y = point.y;
    const double originX = node.partition.x;
    const double originY = node.partition.y;
    const double dx = node.direction.x;
    const double dy = node.direction.y;

    // Doom settles vertical and horizontal partitions apart, which decides on which side a point lying on them is.
    if (dx == 0)
        return x <= originX ? dy > 0 : dy < 0;
    if (dy == 0)
        return y <= originY ? dx < 0 : dx > 0;
    // The point is in front when (y - originY) * dx < dy * (x - originX), and behind otherwise, line included.
    return (y - originY) * dx >= dy * (x - originX);
}
} // namespace

const game::loader::Sector &game::system::GetSubsectorSector(const game::loader::Level &level,
                                                             const game::loader::SubSector &subsector)
{
    const game::loader::Seg &firstSeg = level.segs[subsector.firstSeg];
    const game::loader::Linedef &linedef = level.linedefs[firstSeg.linedef];
    const std::int32_t side = firstSeg.direction == 0 ? linedef.frontSidedef : linedef.backSidedef;

    return level.sectors[level.sidedefs[side].sector];
}

std::size_t game::system::PointInSubsector(const game::loader::Level &level, glm::vec2 point)
{
    if (level.nodes.empty())
        return 0;
    // The root is the last node.
    std::uint32_t child = static_cast<std::uint32_t>(level.nodes.size() - 1);
    while ((child & SUBSECTOR_FLAG) == 0)
    {
        const game::loader::Node &node = level.nodes[child];
        child = IsOnBackSide(point, node) ? node.leftChild : node.rightChild;
    }
    return child & ~SUBSECTOR_FLAG;
}

const game::loader::Sector &game::system::PointInSector(const game::loader::Level &level, glm::vec2 point)
{
    return GetSubsectorSector(level, level.subsectors[PointInSubsector(level, point)]);
}
