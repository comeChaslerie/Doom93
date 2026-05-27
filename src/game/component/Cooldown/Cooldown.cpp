#include "game/component/Cooldown/Cooldown.hpp"
#include "game/component/Seconds/Seconds.hpp"
#include <algorithm>

using namespace game::component;

bool Cooldown::Ready() const { return (_remaining <= Seconds{0.f}); }

void Cooldown::Tick(float dt)
{
    if (_remaining.Value() > 0.f)
        _remaining = Seconds{std::max(_remaining.Value() - dt, 0.f)};
}

void Cooldown::Trigger() { _remaining = _duration; }

Seconds Cooldown::Duration() const { return _duration; }

Seconds Cooldown::Remaining() const { return _remaining; }
