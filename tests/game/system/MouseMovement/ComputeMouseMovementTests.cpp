#include "game/system/MouseMovement/ComputeMouseMovement.hpp"
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <gtest/gtest.h>
#include <numbers>

using game::system::ComputeMouseMovement;

namespace {
constexpr float PI = std::numbers::pi_v<float>;
const glm::quat IDENTITY = glm::quat(1.f, 0.f, 0.f, 0.f);
const glm::vec3 FORWARD = glm::vec3(0.f, 0.f, 1.f);

// On teste l'EFFET de la rotation sur le vecteur "avant" plutot que de comparer
// des quaternions (q et -q representent la meme rotation -> piege).
void ExpectVec(const glm::vec3 &v, float x, float y, float z)
{
    EXPECT_NEAR(v.x, x, 1e-5f);
    EXPECT_NEAR(v.y, y, 1e-5f);
    EXPECT_NEAR(v.z, z, 1e-5f);
}
} // namespace

TEST(ComputeMouseMovement, ZeroDeltaKeepsForward)
{
    glm::quat rotation = ComputeMouseMovement(IDENTITY, 0.f);

    ExpectVec(rotation * FORWARD, 0.f, 0.f, 1.f); // avant inchange
}

TEST(ComputeMouseMovement, QuarterTurnRotatesForward)
{
    glm::quat rotation = ComputeMouseMovement(IDENTITY, PI / 2.f); // +90 deg autour de Y

    ExpectVec(rotation * FORWARD, 1.f, 0.f, 0.f); // (0,0,1) -> (1,0,0)
}

TEST(ComputeMouseMovement, HalfTurnFlipsForward)
{
    glm::quat rotation = ComputeMouseMovement(IDENTITY, PI);

    ExpectVec(rotation * FORWARD, 0.f, 0.f, -1.f);
}

TEST(ComputeMouseMovement, NegativeDeltaRotatesOppositeWay)
{
    glm::quat rotation = ComputeMouseMovement(IDENTITY, -PI / 2.f);

    ExpectVec(rotation * FORWARD, -1.f, 0.f, 0.f); // oppose du +90 deg
}

TEST(ComputeMouseMovement, ResultStaysNormalized)
{
    glm::quat rotation = ComputeMouseMovement(IDENTITY, 12.5f); // delta arbitraire

    EXPECT_NEAR(glm::length(rotation), 1.f, 1e-5f);
}

TEST(ComputeMouseMovement, DeltasAccumulate)
{
    // pi/4 applique deux fois == pi/2 applique une fois (sur l'avant)
    glm::quat once = ComputeMouseMovement(IDENTITY, PI / 2.f);
    glm::quat twice = ComputeMouseMovement(ComputeMouseMovement(IDENTITY, PI / 4.f), PI / 4.f);

    const glm::vec3 expected = once * FORWARD;
    ExpectVec(twice * FORWARD, expected.x, expected.y, expected.z);
}
