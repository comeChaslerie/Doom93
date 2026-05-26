#pragma once

namespace game::component {
class Stat {
  public:
    explicit Stat(float max);
    void Damage(float amount);
    void Heal(float amount);
    void SetMax(float max, bool refill = false);
    [[nodiscard]] float Current() const;
    [[nodiscard]] float Max() const;
    [[nodiscard]] float Ratio() const;
    [[nodiscard]] bool IsEmpty() const;

  private:
    float _current;
    float _max;
    void Clamp();
};
} // namespace game::component
