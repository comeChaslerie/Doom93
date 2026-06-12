#include "component/Transform.hpp"
#include "game/component/Keys/KeysType.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "game/ressources/Keys/Keys.hpp"
#include "game/system/Movement/ComputeMovementVelocity.hpp"
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <gtest/gtest.h>
#include <numbers>

using game::component::KeysType;
using game::component::Velocity;
using game::ressource::Keys;
using game::system::ComputeMovementVelocity;
using Object::Component::Transform;

namespace {
constexpr float PI = std::numbers::pi_v<float>;

// ComputeMovementVelocity lit les 4 directions via Keys::GetKeyState (.at()) ->
// les 4 entrees doivent exister, sinon ca leve.
Keys MakeKeys(bool up, bool down, bool left, bool right)
{
    Keys keys;
    keys.SetKeyState(KeysType::UP_Key, up);
    keys.SetKeyState(KeysType::DOWN_Key, down);
    keys.SetKeyState(KeysType::LEFT_Key, left);
    keys.SetKeyState(KeysType::RIGHT_Key, right);
    return keys;
}

void ExpectVec(const glm::vec3 &v, float x, float y, float z)
{
    EXPECT_NEAR(v.x, x, 1e-5f);
    EXPECT_NEAR(v.y, y, 1e-5f);
    EXPECT_NEAR(v.z, z, 1e-5f);
}
} // namespace

// Rotation identite : forward = (0,0,1), right = (1,0,0) (plan XZ, le sol).

TEST(ComputeMovementVelocity, UpFollowsForward)
{
    Keys keys = MakeKeys(true, false, false, false);
    Transform transform;
    Velocity vel;

    ComputeMovementVelocity(vel, keys, transform);

    ExpectVec(vel.movement, 0.f, 0.f, 1.f);
}

TEST(ComputeMovementVelocity, DownIsBackward)
{
    Keys keys = MakeKeys(false, true, false, false);
    Transform transform;
    Velocity vel;

    ComputeMovementVelocity(vel, keys, transform);

    ExpectVec(vel.movement, 0.f, 0.f, -1.f);
}

TEST(ComputeMovementVelocity, LeftStrafesNegativeX)
{
    Keys keys = MakeKeys(false, false, true, false);
    Transform transform;
    Velocity vel;

    ComputeMovementVelocity(vel, keys, transform);

    ExpectVec(vel.movement, -1.f, 0.f, 0.f);
}

TEST(ComputeMovementVelocity, RightStrafesPositiveX)
{
    Keys keys = MakeKeys(false, false, false, true);
    Transform transform;
    Velocity vel;

    ComputeMovementVelocity(vel, keys, transform);

    ExpectVec(vel.movement, 1.f, 0.f, 0.f);
}

TEST(ComputeMovementVelocity, NoKeyStaysZero)
{
    Keys keys = MakeKeys(false, false, false, false);
    Transform transform;
    Velocity vel;

    ComputeMovementVelocity(vel, keys, transform);

    ExpectVec(vel.movement, 0.f, 0.f, 0.f);
}

TEST(ComputeMovementVelocity, DiagonalIsNormalized)
{
    Keys keys = MakeKeys(true, false, true, false); // avant + strafe gauche
    Transform transform;
    Velocity vel;

    ComputeMovementVelocity(vel, keys, transform);

    EXPECT_NEAR(glm::length(vel.movement), 1.f, 1e-5f); // longueur 1, pas sqrt(2)
}

TEST(ComputeMovementVelocity, OppositeKeysCancelWithoutNaN)
{
    Keys keys = MakeKeys(true, true, false, false); // avant + arriere
    Transform transform;
    Velocity vel;

    ComputeMovementVelocity(vel, keys, transform);

    EXPECT_FALSE(std::isnan(vel.movement.x)); // le garde doit renvoyer {0,0,0}, pas un NaN
    EXPECT_FALSE(std::isnan(vel.movement.y));
    EXPECT_FALSE(std::isnan(vel.movement.z));
    ExpectVec(vel.movement, 0.f, 0.f, 0.f);
}

TEST(ComputeMovementVelocity, MovementIsRelativeToFacing)
{
    // Joueur tourne de +90 deg autour de Y -> forward devient (1,0,0).
    // Donc "avancer" doit deplacer vers +X, pas vers +Z.
    Keys keys = MakeKeys(true, false, false, false);
    Transform transform(glm::vec3(0.f), glm::vec3(1.f), glm::angleAxis(PI / 2.f, glm::vec3(0.f, 1.f, 0.f)));
    Velocity vel;

    ComputeMovementVelocity(vel, keys, transform);

    ExpectVec(vel.movement, 1.f, 0.f, 0.f);
}
