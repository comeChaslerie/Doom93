#pragma once

namespace game::component {
class IDamageModifier {
  public:
    virtual ~IDamageModifier() = default;
    [[nodiscard]] virtual float Modify(float damage) const = 0;
};
} // namespace game::component
