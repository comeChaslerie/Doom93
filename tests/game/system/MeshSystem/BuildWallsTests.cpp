#include "game/loader/LumpsData.hpp"
#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"
#include <glm/vec3.hpp>
#include <gtest/gtest.h>

using game::system::BuildWalls;

namespace {
// Mini-niveau : 3 sommets, 1 secteur (sol 0, plafond 100), 1 sidedef, 2 linedefs.
//   linedef 0 : (0,0) -> (10,0)
//   linedef 1 : (10,0) -> (10,10)
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

    game::loader::Linedef l0;
    l0.startVertex = 0;
    l0.endVertex = 1;
    l0.frontSidedef = 0;
    game::loader::Linedef l1;
    l1.startVertex = 1;
    l1.endVertex = 2;
    l1.frontSidedef = 0;
    level.linedefs = {l0, l1};
    return level;
}

void ExpectVec3(const glm::vec3 &v, float x, float y, float z)
{
    EXPECT_FLOAT_EQ(v.x, x);
    EXPECT_FLOAT_EQ(v.y, y);
    EXPECT_FLOAT_EQ(v.z, z);
}
} // namespace

TEST(BuildWalls, OneQuadPerLinedef)
{
    auto mesh = BuildWalls(MakeLevel());

    // 2 linedefs -> 2 quads -> 8 sommets, 12 indices, et autant de normales/UV.
    EXPECT_EQ(mesh.GetVertices().size(), 8U);
    EXPECT_EQ(mesh.GetIndices().size(), 12U);
    EXPECT_EQ(mesh.GetNormals().size(), 8U);
    EXPECT_EQ(mesh.GetTexCoords().size(), 8U);
}

TEST(BuildWalls, FirstQuadPositions)
{
    auto mesh = BuildWalls(MakeLevel());
    const auto &v = mesh.GetVertices();

    // linedef 0 : (0,0)->(10,0), sol 0 / plafond 100. Mapping (mapX, h, mapY).
    ExpectVec3(v[0], 0.f, 0.f, 0.f);    // start, sol
    ExpectVec3(v[1], 10.f, 0.f, 0.f);   // end, sol
    ExpectVec3(v[2], 10.f, 100.f, 0.f); // end, plafond
    ExpectVec3(v[3], 0.f, 100.f, 0.f);  // start, plafond
}

TEST(BuildWalls, SecondQuadUsesItsOwnVertexes)
{
    auto mesh = BuildWalls(MakeLevel());
    const auto &v = mesh.GetVertices();

    // linedef 1 : (10,0)->(10,10). Mur dans le plan X=10, le long de Z.
    ExpectVec3(v[4], 10.f, 0.f, 0.f);
    ExpectVec3(v[5], 10.f, 0.f, 10.f);
    ExpectVec3(v[6], 10.f, 100.f, 10.f);
    ExpectVec3(v[7], 10.f, 100.f, 0.f);
}

TEST(BuildWalls, IndicesUseAccumulatedBase)
{
    auto mesh = BuildWalls(MakeLevel());

    // Chaque quad : {base, base+1, base+2, base+2, base+3, base}.
    // Le 2e quad doit demarrer a base = 4 (et non 0) -> preuve de l'accumulation.
    const std::vector<uint32_t> expected = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};
    EXPECT_EQ(mesh.GetIndices(), expected);
}
