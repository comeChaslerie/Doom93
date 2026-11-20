#include "game/loader/LumpsData.hpp"
#include "game/system/MeshSystem/BuildFloorCeil/BuildFloorCeil.hpp"
#include <cmath>
#include <glm/vec3.hpp>
#include <gtest/gtest.h>
#include <map>
#include <string>

using game::system::BuildFloorCeil;

namespace {
// Mini-level: 1 triangular subsector (3 segs forming a closed loop), sector with
// floor 0 / ceiling 100, and DISTINCT texture names to verify the grouping.
// Polygon vertices wound clockwise like in a real WAD (interior on the right of
// each seg): (0,0), (10,10), (10,0).
game::loader::Level MakeLevel()
{
    game::loader::Level level;
    level.vertexes = {
        {0,  0 },
        {10, 10},
        {10, 0 }
    };

    game::loader::Sector sector;
    sector.floorHeight = 0;
    sector.ceilingHeight = 100;
    sector.floorTexture = "FLOOR";
    sector.ceilingTexture = "CEIL";
    level.sectors.push_back(sector);

    game::loader::Sidedef sidedef;
    sidedef.sector = 0;
    level.sidedefs.push_back(sidedef);

    game::loader::Linedef linedef;
    linedef.frontSidedef = 0;
    level.linedefs.push_back(linedef);

    for (int v = 0; v < 3; v++)
    {
        game::loader::Seg seg;
        seg.startVertex = v;
        seg.endVertex = (v + 1) % 3;
        seg.linedef = 0;
        seg.direction = 0;
        level.segs.push_back(seg);
    }

    game::loader::SubSector subsector;
    subsector.segCount = 3;
    subsector.firstSeg = 0;
    level.subsectors.push_back(subsector);
    return level;
}

// Polygons are rebuilt by clipping: vertex order may change, so check presence
// instead of position in the array.
bool ContainsVertex(const std::vector<glm::vec3> &vertices, float x, float y, float z)
{
    for (const auto &v : vertices)
        if (std::abs(v.x - x) < 0.01f && std::abs(v.y - y) < 0.01f && std::abs(v.z - z) < 0.01f)
            return true;
    return false;
}

void ExpectVec3(const glm::vec3 &v, float x, float y, float z)
{
    EXPECT_FLOAT_EQ(v.x, x);
    EXPECT_FLOAT_EQ(v.y, y);
    EXPECT_FLOAT_EQ(v.z, z);
}
} // namespace

TEST(BuildFloorCeil, OneMeshPerTexture)
{
    auto meshes = BuildFloorCeil(MakeLevel());

    // Floor and ceiling use distinct names -> two separate groups/meshes.
    EXPECT_EQ(meshes.size(), 2U);
    EXPECT_EQ(meshes.count("FLOOR"), 1U);
    EXPECT_EQ(meshes.count("CEIL"), 1U);
}

TEST(BuildFloorCeil, FloorMeshAtFloorHeight)
{
    auto meshes = BuildFloorCeil(MakeLevel());
    const auto &floor = meshes.at("FLOOR");

    EXPECT_EQ(floor.GetVertices().size(), 3U); // 1 triangle
    EXPECT_EQ(floor.GetIndices().size(), 3U);
    EXPECT_TRUE(ContainsVertex(floor.GetVertices(), 0.f, 0.f, 0.f)); // y = floorHeight
    EXPECT_TRUE(ContainsVertex(floor.GetVertices(), 10.f, 0.f, 10.f));
    EXPECT_TRUE(ContainsVertex(floor.GetVertices(), 10.f, 0.f, 0.f));
    ExpectVec3(floor.GetNormals()[0], 0.f, 1.f, 0.f); // floor -> normal points up
}

TEST(BuildFloorCeil, CeilingMeshAtCeilingHeight)
{
    auto meshes = BuildFloorCeil(MakeLevel());
    const auto &ceil = meshes.at("CEIL");

    EXPECT_EQ(ceil.GetVertices().size(), 3U);
    EXPECT_TRUE(ContainsVertex(ceil.GetVertices(), 0.f, 100.f, 0.f)); // y = ceilingHeight
    ExpectVec3(ceil.GetNormals()[0], 0.f, -1.f, 0.f);                 // ceiling -> normal points down
}

TEST(BuildFloorCeil, OppositeWindingPerFace)
{
    auto meshes = BuildFloorCeil(MakeLevel());

    // Each mesh has its own numbering (base 0). Opposite windings.
    const std::vector<uint32_t> ceilExpected = {0, 1, 2};
    const std::vector<uint32_t> floorExpected = {0, 2, 1};
    EXPECT_EQ(meshes.at("CEIL").GetIndices(), ceilExpected);
    EXPECT_EQ(meshes.at("FLOOR").GetIndices(), floorExpected);
}

TEST(BuildFloorCeil, SingleSegSubsectorsProduceGeometry)
{
    // Square (0,0)-(64,64) split by the partition x=32: two leaves with a single
    // seg each. Before the BSP-plane reconstruction they produced no triangle at
    // all (holes in floors/ceilings).
    game::loader::Level level;
    level.vertexes = {
        {0,  0 },
        {0,  64},
        {64, 64},
        {64, 0 }
    };

    game::loader::Sector sector;
    sector.floorHeight = 0;
    sector.ceilingHeight = 100;
    sector.floorTexture = "FLOOR";
    sector.ceilingTexture = "CEIL";
    level.sectors.push_back(sector);

    game::loader::Sidedef sidedef;
    sidedef.sector = 0;
    level.sidedefs.push_back(sidedef);

    game::loader::Linedef linedef;
    linedef.frontSidedef = 0;
    level.linedefs.push_back(linedef);

    game::loader::Seg right;
    right.startVertex = 2; // (64,64) -> (64,0), interior on its right
    right.endVertex = 3;
    level.segs.push_back(right);
    game::loader::Seg left;
    left.startVertex = 0; // (0,0) -> (0,64), interior on its right
    left.endVertex = 1;
    level.segs.push_back(left);

    for (int firstSeg = 0; firstSeg < 2; firstSeg++)
    {
        game::loader::SubSector subsector;
        subsector.segCount = 1;
        subsector.firstSeg = firstSeg;
        level.subsectors.push_back(subsector);
    }

    game::loader::Node node;
    node.partition = {32, 0};
    node.direction = {0, 64};
    node.rightChild = 0x8000 | 0;
    node.leftChild = 0x8000 | 1;
    level.nodes.push_back(node);

    auto meshes = BuildFloorCeil(level);

    // Two rectangles of 4 vertices / 2 triangles each, per face.
    EXPECT_EQ(meshes.at("FLOOR").GetVertices().size(), 8U);
    EXPECT_EQ(meshes.at("FLOOR").GetIndices().size(), 12U);
    EXPECT_EQ(meshes.at("CEIL").GetVertices().size(), 8U);
    EXPECT_EQ(meshes.at("CEIL").GetIndices().size(), 12U);
}
