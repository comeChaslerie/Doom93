#include "game/component/Keys/KeysType.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "game/ressources/Keys/Keys.hpp"
#include "game/system/Movement/ComputeMovementVelocity.hpp"
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <gtest/gtest.h>

using game::component::KeysType;
using game::component::Velocity;
using game::ressource::Keys;
using game::system::ComputeMovementVelocity;

namespace {
// ComputeMovementVelocity lit l'etat des 4 directions via Keys::GetKeyState,
// qui fait un .at() -> il faut que les 4 entrees existent, sinon ca leve.
Keys MakeKeys(bool up, bool down, bool left, bool right)
{
    Keys keys;
    keys.SetKeyState(KeysType::UP_Key, up);
    keys.SetKeyState(KeysType::DOWN_Key, down);
    keys.SetKeyState(KeysType::LEFT_Key, left);
    keys.SetKeyState(KeysType::RIGHT_Key, right);
    return keys;
}

// cos/sin laissent des residus (~1e-16) -> tolerance plutot que FLOAT_EQ.
void ExpectVec(const glm::vec3 &v, float x, float y, float z)
{
    EXPECT_NEAR(v.x, x, 1e-5f);
    EXPECT_NEAR(v.y, y, 1e-5f);
    EXPECT_NEAR(v.z, z, 1e-5f);
}
} // namespace

TEST(ComputeMovementVelocity, UpGoesPositiveX)
{
    Keys keys = MakeKeys(true, false, false, false);
    Velocity vel;

    ComputeMovementVelocity(vel, keys);

    ExpectVec(vel.movement, 1.f, 0.f, 0.f);
}

TEST(ComputeMovementVelocity, DownGoesNegativeX)
{
    Keys keys = MakeKeys(false, true, false, false);
    Velocity vel;

    ComputeMovementVelocity(vel, keys);

    ExpectVec(vel.movement, -1.f, 0.f, 0.f);
}

TEST(ComputeMovementVelocity, LeftGoesPositiveY)
{
    Keys keys = MakeKeys(false, false, true, false);
    Velocity vel;

    ComputeMovementVelocity(vel, keys);

    ExpectVec(vel.movement, 0.f, 1.f, 0.f);
}

TEST(ComputeMovementVelocity, RightGoesNegativeY)
{
    Keys keys = MakeKeys(false, false, false, true);
    Velocity vel;

    ComputeMovementVelocity(vel, keys);

    ExpectVec(vel.movement, 0.f, -1.f, 0.f);
}

TEST(ComputeMovementVelocity, NoKeyStaysZero)
{
    Keys keys = MakeKeys(false, false, false, false);
    Velocity vel;

    ComputeMovementVelocity(vel, keys);

    ExpectVec(vel.movement, 0.f, 0.f, 0.f);
}

TEST(ComputeMovementVelocity, DiagonalIsNormalized)
{
    Keys keys = MakeKeys(true, false, true, false); // avant + gauche
    Velocity vel;

    ComputeMovementVelocity(vel, keys);

    // longueur 1, pas sqrt(2) : la diagonale ne va pas plus vite.
    EXPECT_NEAR(glm::length(vel.movement), 1.f, 1e-5f);
    ExpectVec(vel.movement, 0.70710678f, 0.70710678f, 0.f);
}

TEST(ComputeMovementVelocity, OppositeKeysCancelWithoutNaN)
{
    Keys keys = MakeKeys(true, true, false, false); // avant + arriere
    Velocity vel;

    ComputeMovementVelocity(vel, keys);

    // La somme est ~nulle : le garde epsilon doit renvoyer {0,0,0}, pas un NaN
    // (normalize d'un vecteur quasi-nul amplifierait le residu).
    EXPECT_FALSE(std::isnan(vel.movement.x));
    EXPECT_FALSE(std::isnan(vel.movement.y));
    EXPECT_FALSE(std::isnan(vel.movement.z));
    ExpectVec(vel.movement, 0.f, 0.f, 0.f);
}
