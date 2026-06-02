#pragma once

#include "game/component/Damage/IDamageModifier.hpp"

namespace game::component {
class FlatBonus : public IDamageModifier {
  public:
    explicit FlatBonus(float bonus) : _bonus(bonus) {}
    [[nodiscard]] float Modify(float damage) const override { return damage + _bonus; }

  private:
    float _bonus;
};

class Multiplier : public IDamageModifier {
  public:
    explicit Multiplier(float factor) : _factor(factor) {}
    [[nodiscard]] float Modify(float damage) const override { return damage * _factor; }

  private:
    float _factor;
};
} // namespace game::component
