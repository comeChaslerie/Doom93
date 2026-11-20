#include "game/system/MeshSystem/BuildFloorCeil/SubsectorPolygon.hpp"
#include "game/loader/LumpsData.hpp"
#include <cmath>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace {
using Polygon = std::vector<glm::dvec2>;

constexpr double kOnLineEps = 0.01; // map units; vertices closer than this merge
constexpr double kMinArea = 0.5;    // slivers below this are invisible, drop them
constexpr std::uint32_t kLeafFlag = 0x8000;

// Signed distance from point to the line (origin, unit direction): < 0 = right side.
double SideDistance(const glm::dvec2 &point, const glm::dvec2 &origin, const glm::dvec2 &unitDir)
{
    glm::dvec2 to = point - origin;

    return unitDir.x * to.y - unitDir.y * to.x;
}

// Sutherland-Hodgman against one line, keeping the right side: a subsector lies
// on the right of its segs and of the partitions it descends the front side of.
Polygon ClipKeepRight(const Polygon &polygon, const glm::dvec2 &origin, const glm::dvec2 &direction)
{
    double length = glm::length(direction);

    if (length < kOnLineEps) // degenerate line: nothing to clip against
        return polygon;

    glm::dvec2 unitDir = direction / length;
    Polygon clipped;

    for (std::size_t i = 0; i < polygon.size(); i++)
    {
        const glm::dvec2 &current = polygon[i];
        const glm::dvec2 &next = polygon[(i + 1) % polygon.size()];
        double distCurrent = SideDistance(current, origin, unitDir);
        double distNext = SideDistance(next, origin, unitDir);

        if (distCurrent <= kOnLineEps)
            clipped.push_back(current);
        if ((distCurrent > kOnLineEps) != (distNext > kOnLineEps))
            clipped.push_back(current + (next - current) * (distCurrent / (distCurrent - distNext)));
    }
    return clipped;
}

Polygon RemoveDuplicates(const Polygon &polygon)
{
    Polygon deduped;

    for (const auto &point : polygon)
        if (deduped.empty() || glm::length(point - deduped.back()) > kOnLineEps)
            deduped.push_back(point);
    if (deduped.size() > 1 && glm::length(deduped.front() - deduped.back()) <= kOnLineEps)
        deduped.pop_back();
    return deduped;
}

double Area(const Polygon &polygon)
{
    double area = 0.0;

    for (std::size_t i = 0; i < polygon.size(); i++)
    {
        const glm::dvec2 &a = polygon[i];
        const glm::dvec2 &b = polygon[(i + 1) % polygon.size()];
        area += a.x * b.y - b.x * a.y;
    }
    return std::abs(area) / 2.0;
}

std::vector<glm::vec2> Finalize(Polygon polygon, const game::loader::SubSector &subsector,
                                const game::loader::Level &level)
{
    for (int i = 0; i < subsector.segCount; i++)
    {
        const game::loader::Seg &seg = level.segs[subsector.firstSeg + i];
        glm::dvec2 start = level.vertexes[seg.startVertex];
        glm::dvec2 end = level.vertexes[seg.endVertex];

        polygon = ClipKeepRight(polygon, start, end - start);
    }
    polygon = RemoveDuplicates(polygon);
    if (polygon.size() < 3 || Area(polygon) < kMinArea)
        return {};
    return {polygon.begin(), polygon.end()};
}

void Descend(const game::loader::Level &level, std::uint32_t child, Polygon polygon,
             std::vector<std::vector<glm::vec2>> &polygons)
{
    if (child & kLeafFlag)
    {
        std::uint32_t index = child & ~kLeafFlag;

        if (index < polygons.size())
            polygons[index] = Finalize(std::move(polygon), level.subsectors[index], level);
        return;
    }
    const game::loader::Node &node = level.nodes[child];
    glm::dvec2 origin = node.partition;
    glm::dvec2 direction = node.direction;

    Descend(level, node.rightChild, ClipKeepRight(polygon, origin, direction), polygons);
    Descend(level, node.leftChild, ClipKeepRight(polygon, origin, -direction), polygons);
}

Polygon MapBoundingBox(const std::vector<glm::ivec2> &vertexes)
{
    glm::dvec2 min = vertexes.front();
    glm::dvec2 max = vertexes.front();

    for (const auto &vertex : vertexes)
    {
        min = glm::min(min, glm::dvec2(vertex));
        max = glm::max(max, glm::dvec2(vertex));
    }
    // Clockwise (map coords, y up), so every clipped polygon stays clockwise.
    return {
        {min.x, min.y},
        {min.x, max.y},
        {max.x, max.y},
        {max.x, min.y}
    };
}
} // namespace

std::vector<std::vector<glm::vec2>> game::system::BuildSubsectorPolygons(const game::loader::Level &level)
{
    std::vector<std::vector<glm::vec2>> polygons(level.subsectors.size());

    if (level.subsectors.empty() || level.vertexes.empty())
        return polygons;
    if (level.nodes.empty()) // whole map is a single leaf
        polygons[0] = Finalize(MapBoundingBox(level.vertexes), level.subsectors[0], level);
    else
        Descend(level, static_cast<std::uint32_t>(level.nodes.size() - 1), MapBoundingBox(level.vertexes), polygons);
    return polygons;
}
