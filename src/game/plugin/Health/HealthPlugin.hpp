#pragma once

#include "plugin/APlugin.hpp"
namespace game::plugin {
class HealthPlugin : public Engine::APlugin {
  public:
    using Engine::APlugin::APlugin;
    void Bind() override;
};
} // namespace game::plugin
