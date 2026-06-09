#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "game/component/Velocity/Velocity.hpp"
#include "game/system/Movement/MovementSystem.hpp"
#include "resource/Time.hpp"
#include <glm/vec3.hpp>
#include <gtest/gtest.h>

using game::component::Velocity;
using game::system::MovementSystem;
using Object::Component::Transform;

namespace {
// La resource Time existe deja (creee par le Core) ; on fixe juste un delta
// deterministe au lieu d'appeler Time::Update qui lit l'horloge reelle.
void SetDeltaTime(Engine::Core &core, float seconds)
{
    core.GetResource<Engine::Resource::Time>()._elapsedTime = seconds;
}

const glm::vec3 &PositionOf(Engine::Core &core, Engine::EntityId id)
{
    return core.GetRegistry().get<Transform>(id).GetPosition();
}
} // namespace

TEST(MovementSystem, AdvancesPositionByVelocityTimesDelta)
{
    Engine::Core core;
    SetDeltaTime(core, 0.5f);

    auto entity = core.CreateEntity();
    entity.AddComponent<Transform>(glm::vec3(0.f));
    entity.AddComponent<Velocity>(glm::vec3(2.f, 0.f, 0.f));

    MovementSystem(core);

    const glm::vec3 &pos = PositionOf(core, entity.Id()); // 2 * 0.5 = 1
    EXPECT_FLOAT_EQ(pos.x, 1.f);
    EXPECT_FLOAT_EQ(pos.y, 0.f);
    EXPECT_FLOAT_EQ(pos.z, 0.f);
}

TEST(MovementSystem, MovesOnAllThreeAxes)
{
    Engine::Core core;
    SetDeltaTime(core, 1.f);

    auto entity = core.CreateEntity();
    entity.AddComponent<Transform>(glm::vec3(10.f, 20.f, 30.f));
    entity.AddComponent<Velocity>(glm::vec3(1.f, -2.f, 3.f));

    MovementSystem(core);

    const glm::vec3 &pos = PositionOf(core, entity.Id());
    EXPECT_FLOAT_EQ(pos.x, 11.f);
    EXPECT_FLOAT_EQ(pos.y, 18.f);
    EXPECT_FLOAT_EQ(pos.z, 33.f);
}

TEST(MovementSystem, ZeroDeltaKeepsPositionStill)
{
    Engine::Core core;
    SetDeltaTime(core, 0.f);

    auto entity = core.CreateEntity();
    entity.AddComponent<Transform>(glm::vec3(5.f, 5.f, 5.f));
    entity.AddComponent<Velocity>(glm::vec3(100.f, 100.f, 100.f));

    MovementSystem(core);

    const glm::vec3 &pos = PositionOf(core, entity.Id());
    EXPECT_FLOAT_EQ(pos.x, 5.f);
    EXPECT_FLOAT_EQ(pos.y, 5.f);
    EXPECT_FLOAT_EQ(pos.z, 5.f);
}

TEST(MovementSystem, EntityWithoutVelocityStaysStill)
{
    Engine::Core core;
    SetDeltaTime(core, 1.f);

    // Une camera : un Transform mais pas de Velocity -> ne doit pas bouger.
    auto camera = core.CreateEntity();
    camera.AddComponent<Transform>(glm::vec3(7.f, 0.f, 0.f));

    MovementSystem(core);

    const glm::vec3 &pos = PositionOf(core, camera.Id());
    EXPECT_FLOAT_EQ(pos.x, 7.f);
    EXPECT_FLOAT_EQ(pos.y, 0.f);
    EXPECT_FLOAT_EQ(pos.z, 0.f);
}

TEST(MovementSystem, AccumulatesOverMultipleFrames)
{
    Engine::Core core;
    SetDeltaTime(core, 0.25f);

    auto entity = core.CreateEntity();
    entity.AddComponent<Transform>(glm::vec3(0.f));
    entity.AddComponent<Velocity>(glm::vec3(4.f, 0.f, 0.f));

    MovementSystem(core); // +1
    MovementSystem(core); // +1
    MovementSystem(core); // +1

    const glm::vec3 &pos = PositionOf(core, entity.Id()); // 3 * (4 * 0.25) = 3
    EXPECT_FLOAT_EQ(pos.x, 3.f);
}
