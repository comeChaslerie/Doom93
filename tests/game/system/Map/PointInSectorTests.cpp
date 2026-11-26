#include "game/loader/LumpsData.hpp"
#include "game/loader/WadLoader.hpp"
#include "game/system/Map/PointInSector.hpp"
#include "game/system/MeshSystem/BuildFloorCeil/SubsectorPolygon.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <glm/vec2.hpp>
#include <gtest/gtest.h>
#include <vector>

using game::loader::Level;
using game::system::PointInSector;
using game::system::PointInSubsector;

namespace {
constexpr std::uint32_t SUBSECTOR_FLAG = 0x8000;
constexpr const char *WAD_PATH = "freedoom/freedoom1.wad";

// Un seul noeud : la partition part de origin selon direction, le cote droit est le sous-secteur 1, le gauche le 0.
Level OneNodeLevel(glm::ivec2 origin, glm::ivec2 direction)
{
    Level level;
    game::loader::Node node;

    node.partition = origin;
    node.direction = direction;
    node.rightChild = SUBSECTOR_FLAG | 1U;
    node.leftChild = SUBSECTOR_FLAG | 0U;
    level.nodes.push_back(node);
    return level;
}
} // namespace

TEST(PointInSubsector, WithoutNodesTheOnlySubsector) { EXPECT_EQ(PointInSubsector(Level{}, {12.f, -40.f}), 0U); }

TEST(PointInSubsector, RightOfThePartitionIsTheRightChild)
{
    const Level level = OneNodeLevel({0, 0}, {64, 64}); // diagonale montante : la droite est en dessous

    EXPECT_EQ(PointInSubsector(level, {10.f, 0.f}), 1U);
    EXPECT_EQ(PointInSubsector(level, {0.f, 10.f}), 0U);
}

TEST(PointInSubsector, OnADiagonalPartitionGoesLeft)
{
    // R_PointOnSide : un point sur la ligne est derriere (cote gauche).
    EXPECT_EQ(PointInSubsector(OneNodeLevel({0, 0}, {64, 64}), {5.f, 5.f}), 0U);
}

TEST(PointInSubsector, OnAVerticalPartitionFollowsItsDirection)
{
    // x <= origin.x renvoie dy > 0 : derriere si la partition monte, devant si elle descend.
    EXPECT_EQ(PointInSubsector(OneNodeLevel({128, 0}, {0, 128}), {128.f, 64.f}), 0U);
    EXPECT_EQ(PointInSubsector(OneNodeLevel({128, 128}, {0, -128}), {128.f, 64.f}), 1U);
}

TEST(PointInSubsector, OnAHorizontalPartitionFollowsItsDirection)
{
    // y <= origin.y renvoie dx < 0 : devant si la partition va vers +x, derriere sinon.
    EXPECT_EQ(PointInSubsector(OneNodeLevel({0, 64}, {128, 0}), {64.f, 64.f}), 1U);
    EXPECT_EQ(PointInSubsector(OneNodeLevel({128, 64}, {-128, 0}), {64.f, 64.f}), 0U);
}

TEST(PointInSubsector, FarFromTheLineStaysExactAtMapScale)
{
    // A (32766, 32764) de l'origine, les deux produits compares valent ~2^30 et ne different que de 2 : en float ils
    // s'arrondissent a la meme valeur et le point passerait derriere.
    const Level level = OneNodeLevel({-32000, -32000}, {32767, 32765});

    EXPECT_EQ(PointInSubsector(level, {32766.f - 32000.f, 32764.f - 32000.f}), 1U);
    EXPECT_EQ(PointInSubsector(level, {32766.f - 32000.f, 32765.f - 32000.f}), 0U);
}

TEST(PointInSector, ReturnsTheSectorOfTheSubsector)
{
    Level level = OneNodeLevel({128, 0}, {0, 128});

    level.sectors.resize(2);
    level.sectors[0].floorHeight = 0;
    level.sectors[1].floorHeight = 16;
    level.sidedefs.resize(2);
    level.sidedefs[0].sector = 0;
    level.sidedefs[1].sector = 1;
    // Une ligne a deux cotes, droite = secteur 1 : chaque sous-secteur la reference par un seg dans son sens.
    level.linedefs.push_back({.startVertex = 0, .endVertex = 1, .frontSidedef = 1, .backSidedef = 0});
    level.segs.push_back({.linedef = 0, .direction = 1});
    level.segs.push_back({.linedef = 0, .direction = 0});
    level.subsectors.push_back({.segCount = 1, .firstSeg = 0});
    level.subsectors.push_back({.segCount = 1, .firstSeg = 1});

    EXPECT_EQ(PointInSector(level, {64.f, 64.f}).floorHeight, 0);
    EXPECT_EQ(PointInSector(level, {192.f, 64.f}).floorHeight, 16);
}

TEST(PointInSubsector, FindsEverySubsectorOfE1M1)
{
    if (!std::filesystem::exists(WAD_PATH))
        GTEST_SKIP() << WAD_PATH << " introuvable (cwd=" << std::filesystem::current_path().string() << ")";
    const game::loader::LumpData wad = game::loader::WadLoader(WAD_PATH);
    const Level &level = wad.levels.at(0);
    const std::vector<std::vector<glm::vec2>> polygons = game::system::BuildSubsectorPolygons(level);

    // La moyenne des sommets d'un polygone convexe est a l'interieur : le BSP doit y retrouver son sous-secteur.
    std::size_t checked = 0;
    for (std::size_t index = 0; index < polygons.size(); index++)
    {
        if (polygons[index].empty())
            continue;
        glm::vec2 inside{0.f, 0.f};
        for (const glm::vec2 &vertex : polygons[index])
            inside += vertex;
        inside /= static_cast<float>(polygons[index].size());
        EXPECT_EQ(PointInSubsector(level, inside), index);
        checked++;
    }
    EXPECT_GT(checked, 0U);
}
