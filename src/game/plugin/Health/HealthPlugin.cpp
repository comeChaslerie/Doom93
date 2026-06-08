#include "game/plugin/Health/HealthPlugin.hpp"
#include "game/system/ApplyDamage/ApplyDamage.hpp"
#include "game/system/DeathSystem/DeathCleanup.hpp"
#include "game/system/DeathSystem/DeathSystem.hpp"
#include "plugin/PluginEvent.hpp"
#include "scheduler/Update.hpp"

namespace game::plugin {
void HealthPlugin::Bind()
{
    RequirePlugins<Event::Plugin>();
    RegisterSystems<Engine::Scheduler::Update>(game::system::ApplyDamage, game::system::DeathSystem,
                                               game::system::DeathCleanup);
}
} // namespace game::plugin
