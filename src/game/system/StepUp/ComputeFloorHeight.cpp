#include "game/system/StepUp/ComputeFloorHeight.hpp"
#include "game/system/Map/PointInSector.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <optional>
#include <vector>

namespace {
using game::loader::Level;
using game::loader::Linedef;
using game::loader::Sector;
using game::system::Body;

/// @brief Square box Doom gives a body (tmbbox in P_CheckPosition).
struct Box {
    glm::vec2 min;
    glm::vec2 max;
};

/// @brief Result of trying to put a body somewhere.
struct Attempt {
    /// @brief Whether the body cannot go there.
    bool blocked = false;
    /// @brief Highest floor under the box, among those the body can step on.
    float floor = 0.f;
    /// @brief Linedefs that stopped the body, which it may slide along.
    std::vector<std::size_t> blockingLines;
};

Box BoxAround(glm::vec2 center, float radius) { return {center - radius, center + radius}; }

/// @brief Whether a linedef crosses a box, like PIT_CheckLine: their bounding boxes overlap, and the box has corners on
///   both sides of the line. A box that only touches the line does not cross it.
bool Crosses(const Box &box, glm::vec2 start, glm::vec2 end)
{
    if (box.max.x <= std::min(start.x, end.x) || box.min.x >= std::max(start.x, end.x) ||
        box.max.y <= std::min(start.y, end.y) || box.min.y >= std::max(start.y, end.y))
        return false;

    const glm::dvec2 direction = glm::dvec2(end) - glm::dvec2(start);
    const std::array corners{box.min, box.max, glm::vec2(box.min.x, box.max.y), glm::vec2(box.max.x, box.min.y)};
    bool front = false;
    bool back = false;
    for (const glm::vec2 &corner : corners)
    {
        const double side = direction.x * (corner.y - start.y) - direction.y * (corner.x - start.x);
        front = front || side < 0.;
        back = back || side > 0.;
    }
    return front && back;
}

float DistanceToSegment(glm::vec2 point, glm::vec2 start, glm::vec2 end)
{
    const glm::vec2 segment = end - start;
    const float lengthSquared = glm::dot(segment, segment);
    const float along =
        lengthSquared > 0.f ? std::clamp(glm::dot(point - start, segment) / lengthSquared, 0.f, 1.f) : 0.f;
    return glm::distance(point, start + along * segment);
}

/// @brief Tries to put a body at a position, like P_CheckPosition followed by the height checks of P_TryMove.
Attempt TryPosition(const Level &level, const Body &body, glm::vec2 position)
{
    const float reach = body.feet + game::system::MAX_STEP_HEIGHT;
    const Box box = BoxAround(position, body.radius);
    const Box currentBox = BoxAround(body.position, body.radius);
    const Sector &centerSector = game::system::PointInSector(level, position);

    Attempt attempt;
    attempt.floor = static_cast<float>(centerSector.floorHeight);
    attempt.blocked = attempt.floor > reach;
    float ceiling = static_cast<float>(centerSector.ceilingHeight);

    for (std::size_t index = 0; index < level.linedefs.size(); index++)
    {
        const Linedef &line = level.linedefs[index];
        const glm::vec2 start{level.vertexes[line.startVertex]};
        const glm::vec2 end{level.vertexes[line.endVertex]};

        if (line.backSidedef < 0)
        {
            // A wall. The physics engine stops the capsule, whose circle is narrower than Doom's box, so the circle
            // tells where the body can go.
            if (DistanceToSegment(position, start, end) < body.radius)
            {
                attempt.blocked = true;
                attempt.blockingLines.push_back(index);
            }
            continue;
        }
        if (!Crosses(box, start, end))
            continue;

        bool tooHigh = false;
        for (const std::int32_t sidedef : {line.frontSidedef, line.backSidedef})
        {
            const Sector &sector = level.sectors[level.sidedefs[sidedef].sector];
            ceiling = std::min(ceiling, static_cast<float>(sector.ceilingHeight));
            if (static_cast<float>(sector.floorHeight) <= reach)
                attempt.floor = std::max(attempt.floor, static_cast<float>(sector.floorHeight));
            else
                tooHigh = true;
        }
        // A step too high is a wall too. The capsule may already stand closer to it than Doom's box allows, against a
        // diagonal one, so only a step the move newly reaches stops it.
        if (tooHigh && !Crosses(currentBox, start, end))
        {
            attempt.blocked = true;
            attempt.blockingLines.push_back(index);
        }
    }
    // No room above the step for the body (tmceilingz - tmfloorz < height). Without a step, the physics engine
    // already stops the capsule under a ceiling too low.
    if (attempt.floor > body.feet && ceiling - attempt.floor < body.height)
        attempt.blocked = true;
    return attempt;
}

/// @brief Floor the body reaches with its move, if it can make it: straight ahead, or else slid along one of the
///   linedefs that stop it (P_SlideMove).
std::optional<float> TryMove(const Level &level, const Body &body, glm::vec2 move)
{
    if (move == glm::vec2{0.f, 0.f})
        return std::nullopt;

    const Attempt straight = TryPosition(level, body, body.position + move);
    if (!straight.blocked)
        return straight.floor;
    for (const std::size_t index : straight.blockingLines)
    {
        const Linedef &line = level.linedefs[index];
        const glm::vec2 along =
            glm::normalize(glm::vec2(level.vertexes[line.endVertex] - level.vertexes[line.startVertex]));
        const Attempt slid = TryPosition(level, body, body.position + glm::dot(move, along) * along);
        if (!slid.blocked)
            return slid.floor;
    }
    return std::nullopt;
}
} // namespace

float game::system::ComputeFloorHeight(const Level &level, const Body &body, glm::vec2 move)
{
    if (const std::optional<float> reached = TryMove(level, body, move))
        return *reached;
    // Like mobj->floorz, the body keeps the floor of where it stands when its move fails.
    return TryPosition(level, body, body.position).floor;
}
