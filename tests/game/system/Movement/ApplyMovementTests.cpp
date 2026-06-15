#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "game/component/Speed.hpp"
#include "game/component/Stat/Stat.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "game/system/Movement/ApplyMovement.hpp"
#include "resource/Time.hpp"
#include <glm/vec3.hpp>
#include <gtest/gtest.h>

using game::component::Speed;
using game::component::Stat;
using game::component::Velocity;
using game::system::ApplyMovement;
using Object::Component::Transform;

namespace {
// La resource Time existe deja (creee par le Core) ; on fixe un delta deterministe.
void SetDeltaTime(Engine::Core &core, float seconds)
{
    core.GetResource<Engine::Resource::Time>()._elapsedTime = seconds;
}

const glm::vec3 &PositionOf(Engine::Core &core, Engine::EntityId id)
{
    return core.GetRegistry().get<Transform>(id).GetPosition();
}
} // namespace

TEST(ApplyMovement, ScalesByDeltaAndSpeed)
{
    Engine::Core core;
    SetDeltaTime(core, 0.5f);

    auto entity = core.CreateEntity();
    entity.AddComponent<Transform>(glm::vec3(0.f));
    entity.AddComponent<Velocity>(glm::vec3(2.f, 0.f, 0.f));
    entity.AddComponent<Speed>(Stat<float>(10.f));

    ApplyMovement(core);

    // 2 * 0.5 * 10 = 10
    EXPECT_FLOAT_EQ(PositionOf(core, entity.Id()).x, 10.f);
}

TEST(ApplyMovement, MovesOnAllThreeAxes)
{
    Engine::Core core;
    SetDeltaTime(core, 1.f);

    auto entity = core.CreateEntity();
    entity.AddComponent<Transform>(glm::vec3(10.f, 20.f, 30.f));
    entity.AddComponent<Velocity>(glm::vec3(1.f, -2.f, 3.f));
    entity.AddComponent<Speed>(Stat<float>(2.f));

    ApplyMovement(core);

    const glm::vec3 &pos = PositionOf(core, entity.Id()); // delta = v * 1 * 2
    EXPECT_FLOAT_EQ(pos.x, 12.f);
    EXPECT_FLOAT_EQ(pos.y, 16.f);
    EXPECT_FLOAT_EQ(pos.z, 36.f);
}

TEST(ApplyMovement, DefaultSpeedIsApplied)
{
    Engine::Core core;
    SetDeltaTime(core, 1.f);

    auto entity = core.CreateEntity();
    entity.AddComponent<Transform>(glm::vec3(0.f));
    entity.AddComponent<Velocity>(glm::vec3(1.f, 0.f, 0.f));
    entity.AddComponent<Speed>(); // defaut 250

    ApplyMovement(core);

    EXPECT_FLOAT_EQ(PositionOf(core, entity.Id()).x, 250.f);
}

TEST(ApplyMovement, ZeroDeltaKeepsPositionStill)
{
    Engine::Core core;
    SetDeltaTime(core, 0.f);

    auto entity = core.CreateEntity();
    entity.AddComponent<Transform>(glm::vec3(5.f, 5.f, 5.f));
    entity.AddComponent<Velocity>(glm::vec3(100.f, 100.f, 100.f));
    entity.AddComponent<Speed>(Stat<float>(10.f));

    ApplyMovement(core);

    const glm::vec3 &pos = PositionOf(core, entity.Id());
    EXPECT_FLOAT_EQ(pos.x, 5.f);
    EXPECT_FLOAT_EQ(pos.y, 5.f);
    EXPECT_FLOAT_EQ(pos.z, 5.f);
}

TEST(ApplyMovement, WithoutSpeedDoesNotMove)
{
    Engine::Core core;
    SetDeltaTime(core, 1.f);

    // La camera : Transform + Velocity mais pas de Speed -> hors de la view -> immobile.
    auto entity = core.CreateEntity();
    entity.AddComponent<Transform>(glm::vec3(7.f, 0.f, 0.f));
    entity.AddComponent<Velocity>(glm::vec3(1.f, 0.f, 0.f));

    ApplyMovement(core);

    EXPECT_FLOAT_EQ(PositionOf(core, entity.Id()).x, 7.f);
}

TEST(ApplyMovement, AccumulatesOverMultipleFrames)
{
    Engine::Core core;
    SetDeltaTime(core, 0.25f);

    auto entity = core.CreateEntity();
    entity.AddComponent<Transform>(glm::vec3(0.f));
    entity.AddComponent<Velocity>(glm::vec3(1.f, 0.f, 0.f));
    entity.AddComponent<Speed>(Stat<float>(4.f));

    ApplyMovement(core); // +1
    ApplyMovement(core); // +1
    ApplyMovement(core); // +1

    // 3 * (1 * 0.25 * 4) = 3
    EXPECT_FLOAT_EQ(PositionOf(core, entity.Id()).x, 3.f);
}
