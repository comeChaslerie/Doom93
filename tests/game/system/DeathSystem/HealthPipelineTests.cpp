#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "game/component/Damage/DamageEvent.hpp"
#include "game/component/Damage/DamageType.hpp"
#include "game/component/Death/DeathEvent.hpp"
#include "game/component/Health.hpp"
#include "game/system/ApplyDamage/ApplyDamage.hpp"
#include "game/system/DeathSystem/DeathCleanup.hpp"
#include "game/system/DeathSystem/DeathSystem.hpp"
#include "plugin/PluginEvent.hpp"
#include "resource/EventManager.hpp"
#include <gtest/gtest.h>

using game::component::DamageEvent;
using game::component::DamageType;
using game::component::DeathEvent;
using game::component::Health;

namespace {
// Ordre du pipeline Vie/Degats/Mort : produire les degats -> tuer -> nettoyer.
void RunPipeline(Engine::Core &core)
{
    game::system::ApplyDamage(core);
    game::system::DeathSystem(core);
    game::system::DeathCleanup(core);
}
} // namespace

TEST(HealthPipeline, LethalDamageKillsAndCleansUp)
{
    Engine::Core core;
    core.AddPlugins<Event::Plugin>();
    int deaths = 0;
    core.GetResource<Event::Resource::EventManager>().RegisterCallback<DeathEvent>(
        [&deaths](const DeathEvent &) { ++deaths; });

    auto entity = core.CreateEntity();
    entity.AddComponent<Health>();
    entity.AddComponent<DamageEvent>(150.f, DamageType::Hitscan);
    const Engine::EntityId id = entity.Id();

    RunPipeline(core);

    EXPECT_FALSE(core.GetRegistry().valid(id)); // detruite
    EXPECT_EQ(deaths, 1);                       // un seul DeathEvent
}

TEST(HealthPipeline, NonLethalDamageSurvives)
{
    Engine::Core core;
    core.AddPlugins<Event::Plugin>();
    int deaths = 0;
    core.GetResource<Event::Resource::EventManager>().RegisterCallback<DeathEvent>(
        [&deaths](const DeathEvent &) { ++deaths; });

    auto entity = core.CreateEntity();
    auto &health = entity.AddComponent<Health>();
    entity.AddComponent<DamageEvent>(30.f, DamageType::Hitscan);
    const Engine::EntityId id = entity.Id();

    RunPipeline(core);

    EXPECT_TRUE(core.GetRegistry().valid(id));
    EXPECT_FLOAT_EQ(health.hp.Current(), 70.f);
    EXPECT_EQ(deaths, 0);
}

TEST(HealthPipeline, UniformForPlayerAndEnemy)
{
    Engine::Core core;
    core.AddPlugins<Event::Plugin>();
    int deaths = 0;
    core.GetResource<Event::Resource::EventManager>().RegisterCallback<DeathEvent>(
        [&deaths](const DeathEvent &) { ++deaths; });

    auto player = core.CreateEntity();
    player.AddComponent<Health>();
    player.AddComponent<DamageEvent>(150.f, DamageType::Hitscan);
    auto enemy = core.CreateEntity();
    enemy.AddComponent<Health>();
    enemy.AddComponent<DamageEvent>(999.f, DamageType::Projectile);
    const Engine::EntityId idPlayer = player.Id();
    const Engine::EntityId idEnemy = enemy.Id();

    RunPipeline(core);

    EXPECT_FALSE(core.GetRegistry().valid(idPlayer));
    EXPECT_FALSE(core.GetRegistry().valid(idEnemy));
    EXPECT_EQ(deaths, 2);
}

TEST(HealthPipeline, SecondFrameDoesNotDoubleCount)
{
    Engine::Core core;
    core.AddPlugins<Event::Plugin>();
    int deaths = 0;
    core.GetResource<Event::Resource::EventManager>().RegisterCallback<DeathEvent>(
        [&deaths](const DeathEvent &) { ++deaths; });

    auto entity = core.CreateEntity();
    entity.AddComponent<Health>();
    entity.AddComponent<DamageEvent>(150.f, DamageType::Hitscan);

    RunPipeline(core); // frame 1 : mort + cleanup
    RunPipeline(core); // frame 2 : l'entite n'existe plus

    EXPECT_EQ(deaths, 1);
}
