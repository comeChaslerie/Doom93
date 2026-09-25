#include "game/loader/LumpsData.hpp"
#include "game/system/MeshSystem/BuildWalls/BuildWalls.hpp"
#include <glm/vec3.hpp>
#include <gtest/gtest.h>
#include <map>
#include <string>

using game::system::BuildWalls;

namespace {
// Mini-niveau : 3 sommets, 1 secteur, 1 sidedef (texture "WALL"), 2 linedefs
// "une face" qui partagent donc la meme texture -> un seul groupe/mesh.
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
    sidedef.middle = "WALL";
    level.sidedefs.push_back(sidedef);

    // backSidedef = -1 -> mur solide (une face). Sinon BuildWalls le filtre.
    game::loader::Linedef l0;
    l0.startVertex = 0;
    l0.endVertex = 1;
    l0.frontSidedef = 0;
    l0.backSidedef = -1;
    game::loader::Linedef l1;
    l1.startVertex = 1;
    l1.endVertex = 2;
    l1.frontSidedef = 0;
    l1.backSidedef = -1;
    level.linedefs = {l0, l1};
    return level;
}

void ExpectVec3(const glm::vec3 &v, float x, float y, float z)
{
    EXPECT_FLOAT_EQ(v.x, x);
    EXPECT_FLOAT_EQ(v.y, y);
    EXPECT_FLOAT_EQ(v.z, z);
}

// Two sectors joined by one two-sided linedef (0,0) -> (10,0), so the front
// side faces map-south (world -z). Textures: front lower/upper = STEP/DROP,
// back lower/upper = STEP_B/DROP_B.
game::loader::Level MakeTwoSectorLevel(int frontFloor, int frontCeil, int backFloor, int backCeil)
{
    game::loader::Level level;
    level.vertexes = {
        {0,  0},
        {10, 0}
    };

    game::loader::Sector frontSector;
    frontSector.floorHeight = frontFloor;
    frontSector.ceilingHeight = frontCeil;
    game::loader::Sector backSector;
    backSector.floorHeight = backFloor;
    backSector.ceilingHeight = backCeil;
    level.sectors = {frontSector, backSector};

    game::loader::Sidedef front;
    front.sector = 0;
    front.lower = "STEP";
    front.upper = "DROP";
    game::loader::Sidedef back;
    back.sector = 1;
    back.lower = "STEP_B";
    back.upper = "DROP_B";
    level.sidedefs = {front, back};

    game::loader::Linedef linedef;
    linedef.startVertex = 0;
    linedef.endVertex = 1;
    linedef.frontSidedef = 0;
    linedef.backSidedef = 1;
    level.linedefs.push_back(linedef);
    return level;
}
} // namespace

TEST(BuildWalls, GroupedUnderSharedTexture)
{
    auto meshes = BuildWalls(MakeLevel());

    // Les 2 murs partagent "WALL" -> un seul mesh, 2 quads.
    EXPECT_EQ(meshes.size(), 1U);
    const auto &mesh = meshes.at("WALL");
    EXPECT_EQ(mesh.GetVertices().size(), 8U);
    EXPECT_EQ(mesh.GetIndices().size(), 12U);
    EXPECT_EQ(mesh.GetNormals().size(), 8U);
    EXPECT_EQ(mesh.GetTexCoords().size(), 8U);
}

TEST(BuildWalls, FirstQuadPositions)
{
    auto meshes = BuildWalls(MakeLevel());
    const auto &v = meshes.at("WALL").GetVertices();

    // linedef 0 : (0,0)->(10,0), sol 0 / plafond 100. Mapping (mapX, h, mapY).
    ExpectVec3(v[0], 0.f, 0.f, 0.f);    // start, sol
    ExpectVec3(v[1], 10.f, 0.f, 0.f);   // end, sol
    ExpectVec3(v[2], 10.f, 100.f, 0.f); // end, plafond
    ExpectVec3(v[3], 0.f, 100.f, 0.f);  // start, plafond
}

TEST(BuildWalls, IndicesUseAccumulatedBase)
{
    auto meshes = BuildWalls(MakeLevel());

    // 2e quad demarre a base = 4 -> preuve de l'accumulation dans le meme mesh.
    const std::vector<uint32_t> expected = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};
    EXPECT_EQ(meshes.at("WALL").GetIndices(), expected);
}

TEST(BuildWalls, GroupsDifferentTexturesSeparately)
{
    game::loader::Level level;
    level.vertexes = {
        {0,  0 },
        {10, 0 },
        {10, 10}
    };

    game::loader::Sector sector;
    sector.ceilingHeight = 100;
    level.sectors.push_back(sector);

    game::loader::Sidedef a;
    a.sector = 0;
    a.middle = "WALL_A";
    game::loader::Sidedef b;
    b.sector = 0;
    b.middle = "WALL_B";
    level.sidedefs = {a, b};

    game::loader::Linedef l0;
    l0.startVertex = 0;
    l0.endVertex = 1;
    l0.frontSidedef = 0; // texture WALL_A
    l0.backSidedef = -1;
    game::loader::Linedef l1;
    l1.startVertex = 1;
    l1.endVertex = 2;
    l1.frontSidedef = 1; // texture WALL_B
    l1.backSidedef = -1;
    level.linedefs = {l0, l1};

    auto meshes = BuildWalls(level);

    EXPECT_EQ(meshes.size(), 2U);
    EXPECT_EQ(meshes.at("WALL_A").GetVertices().size(), 4U);
    EXPECT_EQ(meshes.at("WALL_B").GetVertices().size(), 4U);
}

TEST(BuildWalls, LowerWallFacesTheLowerFloorSector)
{
    // Front floor below back floor: the step is seen from the front sector.
    auto meshes = BuildWalls(MakeTwoSectorLevel(0, 100, 32, 100));

    ASSERT_EQ(meshes.count("STEP"), 1U);
    const auto &mesh = meshes.at("STEP");
    ASSERT_EQ(mesh.GetVertices().size(), 4U);
    ExpectVec3(mesh.GetVertices()[0], 0.f, 0.f, 0.f); // start, front floor
    ExpectVec3(mesh.GetVertices()[1], 10.f, 0.f, 0.f);
    ExpectVec3(mesh.GetVertices()[2], 10.f, 32.f, 0.f); // end, back floor
    ExpectVec3(mesh.GetVertices()[3], 0.f, 32.f, 0.f);
    ExpectVec3(mesh.GetNormals()[0], 0.f, 0.f, -1.f); // toward the front sector
}

TEST(BuildWalls, LowerWallUsesBackSidedefWhenBackIsLower)
{
    // Back floor below front floor: the back sidedef textures the step and the
    // quad is flipped so its normal points toward the back sector.
    auto meshes = BuildWalls(MakeTwoSectorLevel(32, 100, 0, 100));

    EXPECT_EQ(meshes.count("STEP"), 0U);
    ASSERT_EQ(meshes.count("STEP_B"), 1U);
    const auto &mesh = meshes.at("STEP_B");
    ASSERT_EQ(mesh.GetVertices().size(), 4U);
    ExpectVec3(mesh.GetVertices()[0], 10.f, 0.f, 0.f); // flipped: starts at linedef end
    ExpectVec3(mesh.GetVertices()[2], 0.f, 32.f, 0.f);
    ExpectVec3(mesh.GetNormals()[0], 0.f, 0.f, 1.f); // toward the back sector
}

TEST(BuildWalls, UpperWallSpansTheCeilingDrop)
{
    // Front ceiling above back ceiling: the drop is seen from the front sector.
    auto meshes = BuildWalls(MakeTwoSectorLevel(0, 100, 0, 64));

    ASSERT_EQ(meshes.count("DROP"), 1U);
    const auto &mesh = meshes.at("DROP");
    ASSERT_EQ(mesh.GetVertices().size(), 4U);
    ExpectVec3(mesh.GetVertices()[0], 0.f, 64.f, 0.f);   // start, back ceiling
    ExpectVec3(mesh.GetVertices()[2], 10.f, 100.f, 0.f); // end, front ceiling
    ExpectVec3(mesh.GetNormals()[0], 0.f, 0.f, -1.f);
}

TEST(BuildWalls, StepAndDropCanCoexistOnOneLinedef)
{
    // A door-like opening: raised floor AND lowered ceiling on the back side.
    auto meshes = BuildWalls(MakeTwoSectorLevel(0, 100, 32, 64));

    EXPECT_EQ(meshes.size(), 2U);
    EXPECT_EQ(meshes.count("STEP"), 1U);
    EXPECT_EQ(meshes.count("DROP"), 1U);
}

TEST(BuildWalls, EqualSectorsProduceNoHalfWall)
{
    auto meshes = BuildWalls(MakeTwoSectorLevel(0, 100, 0, 100));

    EXPECT_TRUE(meshes.empty());
}

TEST(BuildWalls, MissingTextureProducesNoQuad)
{
    // "-" is the WAD marker for an absent texture: the height difference alone
    // must not emit geometry.
    auto level = MakeTwoSectorLevel(0, 100, 32, 100);
    level.sidedefs[0].lower = "-";

    auto meshes = BuildWalls(level);

    EXPECT_TRUE(meshes.empty());
}

TEST(BuildWalls, TwoSidedWithEqualSectorsAddsNothing)
{
    game::loader::Level level;
    level.vertexes = {
        {0,  0 },
        {10, 0 },
        {10, 10}
    };

    game::loader::Sector sector;
    sector.ceilingHeight = 100;
    level.sectors.push_back(sector);

    game::loader::Sidedef sidedef;
    sidedef.sector = 0;
    sidedef.middle = "WALL";
    level.sidedefs.push_back(sidedef);

    game::loader::Linedef solid; // une face -> mur
    solid.startVertex = 0;
    solid.endVertex = 1;
    solid.frontSidedef = 0;
    solid.backSidedef = -1;
    game::loader::Linedef opening; // two-sided, same sector on both sides -> no half-wall
    opening.startVertex = 1;
    opening.endVertex = 2;
    opening.frontSidedef = 0;
    opening.backSidedef = 0;
    level.linedefs = {solid, opening};

    auto meshes = BuildWalls(level);

    // Only the solid wall emits geometry -> 1 mesh, 1 quad.
    EXPECT_EQ(meshes.size(), 1U);
    EXPECT_EQ(meshes.at("WALL").GetVertices().size(), 4U);
}
