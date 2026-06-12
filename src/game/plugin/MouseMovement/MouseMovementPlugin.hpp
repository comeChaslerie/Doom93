#pragma once

#include "plugin/APlugin.hpp"

namespace game::plugin {
class MouseMovementPlugin : public Engine::APlugin {
  public:
    using Engine::APlugin::APlugin;
    void Bind() override;
};
} // namespace game::plugin
