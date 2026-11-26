#include "game/loader/LumpsData.hpp"
#include "game/system/StepUp/ComputeFloorHeight.hpp"
#include <cstdint>
#include <glm/vec2.hpp>
#include <gtest/gtest.h>

using game::loader::Level;
using game::system::Body;
using game::system::ComputeFloorHeight;

namespace {
constexpr std::uint32_t SUBSECTOR_FLAG = 0x8000;
// Le joueur de Doom : boite de 32 x 32, 56 de haut.
constexpr float RADIUS = 16.f;
constexpr float HEIGHT = 56.f;
// Ecart que le moteur physique laisse entre la capsule et un mur.
constexpr float PADDING = 0.02f;

// Couloir de 256 x 128 coupe en deux par une marche en x = 128 :
//
//   (0,128) +------------+------------+ (256,128)
//           |   bas      |   haut     |
//           |  sol 0     | sol `step` |
//           |  plafond   | plafond    |
//           |  128       | `ceiling`  |
//     (0,0) +------------+------------+ (256,0)
//
// Les murs exterieurs ont un seul cote, la contremarche deux. Un noeud BSP suit la contremarche.
Level StepLevel(int step, int ceiling = 128)
{
    Level level;

    level.vertexes = {
        {0,   0  },
        {0,   128},
        {128, 128},
        {256, 128},
        {256, 0  },
        {128, 0  }
    };
    level.sectors.resize(2);
    level.sectors[0].floorHeight = 0;
    level.sectors[0].ceilingHeight = 128;
    level.sectors[1].floorHeight = step;
    level.sectors[1].ceilingHeight = ceiling;
    level.sidedefs.resize(2);
    level.sidedefs[0].sector = 0;
    level.sidedefs[1].sector = 1;
    // Interieur a droite de chaque ligne (sens horaire, y vers le haut).
    level.linedefs = {
        {.startVertex = 0, .endVertex = 1, .frontSidedef = 0, .backSidedef = -1}, // mur gauche
        {.startVertex = 1, .endVertex = 2, .frontSidedef = 0, .backSidedef = -1}, // mur du haut, partie basse
        {.startVertex = 2, .endVertex = 3, .frontSidedef = 1, .backSidedef = -1}, // mur du haut, partie haute
        {.startVertex = 3, .endVertex = 4, .frontSidedef = 1, .backSidedef = -1}, // mur droit
        {.startVertex = 4, .endVertex = 5, .frontSidedef = 1, .backSidedef = -1}, // mur du bas, partie haute
        {.startVertex = 5, .endVertex = 0, .frontSidedef = 0, .backSidedef = -1}, // mur du bas, partie basse
        {.startVertex = 5, .endVertex = 2, .frontSidedef = 1, .backSidedef = 0 }, // contremarche
    };
    level.segs = {
        {.linedef = 0, .direction = 0},
        {.linedef = 3, .direction = 0}
    };
    level.subsectors = {
        {.segCount = 1, .firstSeg = 0},
        {.segCount = 1, .firstSeg = 1}
    };
    game::loader::Node node;
    node.partition = {128, 0};
    node.direction = {0, 128};
    node.rightChild = SUBSECTOR_FLAG | 1U; // a droite de la contremarche : le haut
    node.leftChild = SUBSECTOR_FLAG | 0U;
    level.nodes.push_back(node);
    return level;
}

Body BodyAt(glm::vec2 position, float feet)
{
    return {.position = position, .feet = feet, .radius = RADIUS, .height = HEIGHT};
}
} // namespace

TEST(ComputeFloorHeight, StandsOnTheFloorOfItsSector)
{
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(16), BodyAt({64.f, 64.f}, 0.f), {0.f, 0.f}), 0.f);
}

TEST(ComputeFloorHeight, ClimbsAStepTheBoxReaches)
{
    // Boite d'arrivee en x de 99 a 131 : elle chevauche la contremarche.
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(16), BodyAt({100.f, 64.f}, 0.f), {15.f, 0.f}), 16.f);
}

TEST(ComputeFloorHeight, IgnoresAStepTheBoxDoesNotReachYet)
{
    // Boite d'arrivee en x de 84 a 116.
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(16), BodyAt({90.f, 64.f}, 0.f), {10.f, 0.f}), 0.f);
}

TEST(ComputeFloorHeight, ClimbsAStepOf24)
{
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(24), BodyAt({100.f, 64.f}, 0.f), {15.f, 0.f}), 24.f);
}

TEST(ComputeFloorHeight, AStepAbove24IsAWall)
{
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(25), BodyAt({100.f, 64.f}, 0.f), {15.f, 0.f}), 0.f);
}

TEST(ComputeFloorHeight, NeedsRoomForTheBodyAboveTheStep)
{
    // 56 de haut : un plafond a 16 + 55 l'arrete, a 16 + 56 il passe.
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(16, 71), BodyAt({100.f, 64.f}, 0.f), {15.f, 0.f}), 0.f);
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(16, 72), BodyAt({100.f, 64.f}, 0.f), {15.f, 0.f}), 16.f);
}

TEST(ComputeFloorHeight, StaysOnALedgeWhileTheBoxOverlapsIt)
{
    // Centre au-dessus du bas, mais la boite (x de 104 a 136) touche encore le haut.
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(16), BodyAt({120.f, 64.f}, 16.f), {0.f, 0.f}), 16.f);
}

TEST(ComputeFloorHeight, LeavesALedgeOnceTheBoxIsOff)
{
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(16), BodyAt({110.f, 64.f}, 16.f), {0.f, 0.f}), 0.f);
}

TEST(ComputeFloorHeight, TakesTheFloorOfWhereTheMoveEnds)
{
    // La boite touche encore le rebord au depart, plus a l'arrivee (x de 89 a 121) : c'est le sol d'arrivee.
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(16), BodyAt({120.f, 64.f}, 16.f), {-15.f, 0.f}), 0.f);
}

TEST(ComputeFloorHeight, KeepsTheFloorItStandsOnWhenTheMoveFails)
{
    // Sur le rebord a 16, le corps fonce vers un sol a 41, 25 au-dessus de ses pieds : il garde le sol du rebord.
    Level level = StepLevel(16);
    level.sectors[0].floorHeight = 41;

    EXPECT_FLOAT_EQ(ComputeFloorHeight(level, BodyAt({136.f, 64.f}, 16.f), {-15.f, 0.f}), 16.f);
}

TEST(ComputeFloorHeight, CatchesALedgeWhileFalling)
{
    // Pieds a 10 en pleine chute, rebord a 30 : 20 de marche, Doom le monte comme une marche.
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(30), BodyAt({100.f, 64.f}, 10.f), {15.f, 0.f}), 30.f);
}

TEST(ComputeFloorHeight, MissesALedgeMoreThan24AboveTheFeet)
{
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(30), BodyAt({100.f, 64.f}, 5.f), {15.f, 0.f}), 0.f);
}

TEST(ComputeFloorHeight, ATooHighLedgeUnderTheBoxIsNotAFloor)
{
    // La boite deborde deja sur un rebord de 40 : il compte comme un mur, pas comme un sol.
    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(40), BodyAt({120.f, 64.f}, 0.f), {2.f, 0.f}), 0.f);
}

TEST(ComputeFloorHeight, ClimbsAStepAlongTheWallItHugs)
{
    // Colle au mur du haut, le corps avance en biais vers lui : le mouvement droit est bloque, mais glisse le long du
    // mur il atteint la marche, comme la capsule qui glisse.
    const Body body = BodyAt({105.f, 128.f - RADIUS - PADDING}, 0.f);

    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(16), body, {10.f, 5.f}), 16.f);
}

TEST(ComputeFloorHeight, AWallAloneLeavesTheFloorAsItIs)
{
    const Body body = BodyAt({64.f, 128.f - RADIUS - PADDING}, 0.f);

    EXPECT_FLOAT_EQ(ComputeFloorHeight(StepLevel(16), body, {0.f, 10.f}), 0.f);
}
