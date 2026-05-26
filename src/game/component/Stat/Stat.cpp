#include "game/component/Stat/Stat.hpp"
#include <algorithm>

using namespace game::component;

Stat::Stat(float max) : _current(std::max(max, 0.f)), _max(std::max(max, 0.f)) {}

void Stat::Clamp() { _current = std::clamp(_current, 0.f, _max); }

void Stat::Damage(float amount)
{
    _current -= amount;
    Clamp();
}

void Stat::Heal(float amount)
{
    _current += amount;
    Clamp();
}

void Stat::SetMax(float max, bool refill)
{
    _max = std::max(max, 0.f);
    if (refill)
        _current = _max;
    Clamp();
}

float Stat::Current() const { return _current; }

float Stat::Max() const { return _max; }

float Stat::Ratio() const
{
    if (_max == 0)
        return 0.f;
    return _current / _max;
}

bool Stat::IsEmpty() const { return (_current <= 0); }
