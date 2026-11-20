#include "game/loader/LumpsData.hpp"
#include "game/system/MeshSystem/BuildFloorCeil/SubsectorPolygon.hpp"
#include <cmath>
#include <glm/vec2.hpp>
#include <gtest/gtest.h>
#include <vector>

using game::system::BuildSubsectorPolygons;

namespace {
// Shoelace, map coords with y up: negative area = clockwise (Doom winding).
float SignedArea(const std::vector<glm::vec2> &polygon)
{
    float area = 0.f;

    for (std::size_t i = 0; i < polygon.size(); i++)
    {
        const glm::vec2 &a = polygon[i];
        const glm::vec2 &b = polygon[(i + 1) % polygon.size()];
        area += a.x * b.y - b.x * a.y;
    }
    return area / 2.f;
}

bool ContainsVertex(const std::vector<glm::vec2> &polygon, float x, float y)
{
    for (const auto &p : polygon)
        if (std::abs(p.x - x) < 0.01f && std::abs(p.y - y) < 0.01f)
            return true;
    return false;
}

void AddSeg(game::loader::Level &level, int startVertex, int endVertex)
{
    game::loader::Seg seg;

    seg.startVertex = startVertex;
    seg.endVertex = endVertex;
    level.segs.push_back(seg);
}

void AddSubSector(game::loader::Level &level, int segCount, int firstSeg)
{
    game::loader::SubSector subsector;

    subsector.segCount = segCount;
    subsector.firstSeg = firstSeg;
    level.subsectors.push_back(subsector);
}
} // namespace

TEST(BuildSubsectorPolygons, ClosedSegLoopWithoutNodes)
{
    game::loader::Level level;

    // Clockwise triangle (interior on the right of each seg): (0,0) -> (10,10) -> (10,0).
    level.vertexes = {
        {0,  0 },
        {10, 10},
        {10, 0 }
    };
    AddSeg(level, 0, 1);
    AddSeg(level, 1, 2);
    AddSeg(level, 2, 0);
    AddSubSector(level, 3, 0);

    auto polygons = BuildSubsectorPolygons(level);

    ASSERT_EQ(polygons.size(), 1U);
    ASSERT_EQ(polygons[0].size(), 3U);
    EXPECT_TRUE(ContainsVertex(polygons[0], 0.f, 0.f));
    EXPECT_TRUE(ContainsVertex(polygons[0], 10.f, 10.f));
    EXPECT_TRUE(ContainsVertex(polygons[0], 10.f, 0.f));
    EXPECT_FLOAT_EQ(SignedArea(polygons[0]), -50.f);
}

TEST(BuildSubsectorPolygons, SingleSegLeavesRebuiltFromBspPartitions)
{
    game::loader::Level level;

    // Square (0,0)-(64,64) split in two by the vertical partition x=32.
    // Each leaf owns a SINGLE seg (its outer wall): the three other edges are
    // implicit (map border + partition line).
    level.vertexes = {
        {0,  0 },
        {0,  64},
        {64, 64},
        {64, 0 }
    };
    AddSeg(level, 2, 3); // right wall (64,64) -> (64,0), interior on its right (x < 64)
    AddSeg(level, 0, 1); // left wall (0,0) -> (0,64), interior on its right (x > 0)
    AddSubSector(level, 1, 0);
    AddSubSector(level, 1, 1);

    game::loader::Node node;
    node.partition = {32, 0};
    node.direction = {0, 64};
    node.rightChild = 0x8000 | 0; // right side of the partition: x > 32
    node.leftChild = 0x8000 | 1;
    level.nodes.push_back(node);

    auto polygons = BuildSubsectorPolygons(level);

    ASSERT_EQ(polygons.size(), 2U);
    ASSERT_EQ(polygons[0].size(), 4U);
    EXPECT_TRUE(ContainsVertex(polygons[0], 32.f, 0.f));
    EXPECT_TRUE(ContainsVertex(polygons[0], 32.f, 64.f));
    EXPECT_TRUE(ContainsVertex(polygons[0], 64.f, 64.f));
    EXPECT_TRUE(ContainsVertex(polygons[0], 64.f, 0.f));
    EXPECT_FLOAT_EQ(SignedArea(polygons[0]), -2048.f);

    ASSERT_EQ(polygons[1].size(), 4U);
    EXPECT_TRUE(ContainsVertex(polygons[1], 0.f, 0.f));
    EXPECT_TRUE(ContainsVertex(polygons[1], 0.f, 64.f));
    EXPECT_TRUE(ContainsVertex(polygons[1], 32.f, 64.f));
    EXPECT_TRUE(ContainsVertex(polygons[1], 32.f, 0.f));
    EXPECT_FLOAT_EQ(SignedArea(polygons[1]), -2048.f);
}

TEST(BuildSubsectorPolygons, DegenerateSubsectorYieldsEmptyPolygon)
{
    game::loader::Level level;

    // Two opposite segs on the same line: incompatible half-planes, zero area.
    level.vertexes = {
        {0,  32},
        {64, 32}
    };
    AddSeg(level, 0, 1);
    AddSeg(level, 1, 0);
    AddSubSector(level, 2, 0);

    auto polygons = BuildSubsectorPolygons(level);

    ASSERT_EQ(polygons.size(), 1U);
    EXPECT_TRUE(polygons[0].empty());
}
