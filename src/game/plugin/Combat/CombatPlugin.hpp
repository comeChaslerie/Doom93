#pragma once

#include <plugin/APlugin.hpp>

namespace game::plugin {
class CombatPlugin : public Engine::APlugin {
  public:
    using Engine::APlugin::APlugin;
    void Bind() override;
};
} // namespace game::plugin
