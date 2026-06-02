#include "game/plugin/Combat/CombatPlugin.hpp"
#include "game/system/ApplyDamage/ApplyDamage.hpp"
#include "game/system/CooldownSystem/CooldownSystem.hpp"
#include "scheduler/Update.hpp"

namespace game::plugin {
void CombatPlugin::Bind()
{
    RegisterSystems<Engine::Scheduler::Update>(game::system::CooldownSystem, game::system::ApplyDamage);
}
} // namespace game::plugin
