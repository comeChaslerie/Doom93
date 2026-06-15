#include "game/loader/LumpsData.hpp"
#include "game/system/MeshSystem/BuildFloorCeil/BuildFloorCeil.hpp"
#include <glm/vec3.hpp>
#include <gtest/gtest.h>

using game::system::BuildFloorCeil;

namespace {
// Mini-niveau : 1 subsector triangulaire (3 segs), secteur sol 0 / plafond 100.
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

TEST(BuildFloorCeil, FloorAndCeilingPerSubsector)
{
    auto mesh = BuildFloorCeil(MakeLevel());

    // 1 triangle convexe -> eventail = 1 triangle par face. Sol + plafond -> 2 faces.
    // 3 sommets/face -> 6 sommets, 6 indices, 6 normales/UV.
    EXPECT_EQ(mesh.GetVertices().size(), 6U);
    EXPECT_EQ(mesh.GetIndices().size(), 6U);
    EXPECT_EQ(mesh.GetNormals().size(), 6U);
    EXPECT_EQ(mesh.GetTexCoords().size(), 6U);
}

TEST(BuildFloorCeil, CeilingThenFloorHeights)
{
    auto mesh = BuildFloorCeil(MakeLevel());
    const auto &v = mesh.GetVertices();

    // Le plafond est pousse en premier (y=100), puis le sol (y=0).
    ExpectVec3(v[0], 0.f, 100.f, 0.f);
    ExpectVec3(v[3], 0.f, 0.f, 0.f);
}

TEST(BuildFloorCeil, NormalsFaceUpForFloorDownForCeiling)
{
    auto mesh = BuildFloorCeil(MakeLevel());
    const auto &n = mesh.GetNormals();

    ExpectVec3(n[0], 0.f, -1.f, 0.f); // plafond (vers le bas)
    ExpectVec3(n[3], 0.f, 1.f, 0.f);  // sol (vers le haut)
}

TEST(BuildFloorCeil, FloorAndCeilingHaveOppositeWinding)
{
    auto mesh = BuildFloorCeil(MakeLevel());

    // plafond (base 0) : {0,1,2} ; sol (base 3) : winding inverse {3,5,4}.
    const std::vector<uint32_t> expected = {0, 1, 2, 3, 5, 4};
    EXPECT_EQ(mesh.GetIndices(), expected);
}
