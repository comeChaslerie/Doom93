#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "game/component/Tags/Dead.hpp"
#include "game/component/Death/DeathEvent.hpp"
#include "game/component/Health.hpp"
#include "game/system/DeathSystem/DeathSystem.hpp"
#include "plugin/PluginEvent.hpp"
#include "resource/EventManager.hpp"
#include <gtest/gtest.h>

using game::component::Dead;
using game::component::DeathEvent;
using game::component::Health;
using game::system::DeathSystem;

namespace {
std::size_t CountDeadTags(Engine::Core &core) { return core.GetRegistry().view<Dead>().size(); }
} // namespace

TEST(DeathSystem, EmptyHealthGetsTaggedDead)
{
    Engine::Core core;
    core.AddPlugins<Event::Plugin>();
    auto entity = core.CreateEntity();
    auto &health = entity.AddComponent<Health>();
    health.hp.Damage(200.f); // PV a 0 sans passer par ApplyDamage

    DeathSystem(core);

    EXPECT_TRUE(core.GetRegistry().all_of<Dead>(entity.Id()));
}

TEST(DeathSystem, EmitsDeathEventOnce)
{
    Engine::Core core;
    core.AddPlugins<Event::Plugin>();
    int deaths = 0;
    core.GetResource<Event::Resource::EventManager>().RegisterCallback<DeathEvent>(
        [&deaths](const DeathEvent &) { ++deaths; });

    auto entity = core.CreateEntity();
    auto &health = entity.AddComponent<Health>();
    health.hp.Damage(200.f);

    DeathSystem(core);

    EXPECT_EQ(deaths, 1);
}

TEST(DeathSystem, IsIdempotent)
{
    Engine::Core core;
    core.AddPlugins<Event::Plugin>();
    int deaths = 0;
    core.GetResource<Event::Resource::EventManager>().RegisterCallback<DeathEvent>(
        [&deaths](const DeathEvent &) { ++deaths; });

    auto entity = core.CreateEntity();
    auto &health = entity.AddComponent<Health>();
    health.hp.Damage(200.f);

    DeathSystem(core);
    DeathSystem(core); // l'entite est deja Dead -> ignoree par l'exclusion
    DeathSystem(core);

    EXPECT_EQ(CountDeadTags(core), 1U); // un seul tag
    EXPECT_EQ(deaths, 1);               // un seul event
}

TEST(DeathSystem, LivingEntityIsNotTagged)
{
    Engine::Core core;
    core.AddPlugins<Event::Plugin>();
    int deaths = 0;
    core.GetResource<Event::Resource::EventManager>().RegisterCallback<DeathEvent>(
        [&deaths](const DeathEvent &) { ++deaths; });

    auto entity = core.CreateEntity();
    entity.AddComponent<Health>(); // 100 PV, vivant

    DeathSystem(core);

    EXPECT_FALSE(core.GetRegistry().all_of<Dead>(entity.Id()));
    EXPECT_EQ(deaths, 0);
}
