#include "game/loader/LumpsData.hpp"
#include "game/system/MeshSystem/BuildFloorCeil/BuildFloorCeil.hpp"
#include <glm/vec3.hpp>
#include <gtest/gtest.h>
#include <map>
#include <string>

using game::system::BuildFloorCeil;

namespace {
// Mini-niveau : 1 subsector triangulaire (3 segs), secteur sol 0 / plafond 100,
// avec des noms de texture DISTINCTS pour verifier le grouping.
//   sommets du polygone : (0,0), (10,0), (10,10)
game::loader::Level MakeLevel()
{
    game::loader::Level level;
    level.vertexes = {
        {0,  0 },
        {10, 0 },
        {10, 10}
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

    // Sol et plafond ont des noms distincts -> deux groupes/meshes separes.
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
    ExpectVec3(floor.GetVertices()[0], 0.f, 0.f, 0.f); // y = floorHeight
    ExpectVec3(floor.GetNormals()[0], 0.f, 1.f, 0.f);  // sol -> normale vers le haut
}

TEST(BuildFloorCeil, CeilingMeshAtCeilingHeight)
{
    auto meshes = BuildFloorCeil(MakeLevel());
    const auto &ceil = meshes.at("CEIL");

    EXPECT_EQ(ceil.GetVertices().size(), 3U);
    ExpectVec3(ceil.GetVertices()[0], 0.f, 100.f, 0.f); // y = ceilingHeight
    ExpectVec3(ceil.GetNormals()[0], 0.f, -1.f, 0.f);   // plafond -> normale vers le bas
}

TEST(BuildFloorCeil, OppositeWindingPerFace)
{
    auto meshes = BuildFloorCeil(MakeLevel());

    // Chaque mesh a sa propre numerotation (base 0). Windings opposes.
    const std::vector<uint32_t> ceilExpected = {0, 1, 2};
    const std::vector<uint32_t> floorExpected = {0, 2, 1};
    EXPECT_EQ(meshes.at("CEIL").GetIndices(), ceilExpected);
    EXPECT_EQ(meshes.at("FLOOR").GetIndices(), floorExpected);
}
