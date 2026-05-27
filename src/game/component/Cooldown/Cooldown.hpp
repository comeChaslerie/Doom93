#pragma once

#include "game/component/Seconds/Seconds.hpp"
#include <algorithm>

namespace game::component {
class Cooldown {
  public:
    explicit Cooldown(float duration = 0.f) : _remaining(0.f), _duration(Seconds{std::max(0.f, duration)}) {};
    [[nodiscard]] bool Ready() const;
    [[nodiscard]] Seconds Duration() const;
    [[nodiscard]] Seconds Remaining() const;
    void Trigger();
    void Tick(float dt);

  private:
    Seconds _remaining = Seconds{0.f};
    Seconds _duration = Seconds{0.f};
};
} // namespace game::component
