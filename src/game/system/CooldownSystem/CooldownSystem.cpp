#include "game/system/CooldownSystem/CooldownSystem.hpp"
#include "game/component/Cooldown/Cooldown.hpp"
#include "resource/Time.hpp"

void game::system::CooldownSystem(Engine::Core &core)
{
    Engine::Resource::Time &time = core.GetResource<Engine::Resource::Time>();
    float deltaTime = time._elapsedTime;
    auto &registery = core.GetRegistry();

    registery.view<component::Cooldown>().each([&deltaTime](component::Cooldown &cooldown) {
        cooldown.Tick(deltaTime);
    });
}
