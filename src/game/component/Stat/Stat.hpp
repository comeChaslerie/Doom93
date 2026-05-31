#pragma once

#include <algorithm>
#include <concepts>

namespace game::component {
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;
template <Numeric T> class Stat {
  public:
    explicit Stat(T max) : _current(std::max(max, T{})), _max(std::max(max, T{})) {}
    void Damage(T amount)
    {
        _current -= amount;
        Clamp();
    }
    void Heal(T amount)
    {
        _current += amount;
        Clamp();
    }
    void SetMax(T max, bool refill = false)
    {
        _max = std::max(max, T{});
        if (refill)
            _current = _max;
        Clamp();
    }
    [[nodiscard]] T Current() const { return _current; }
    [[nodiscard]] T Max() const { return _max; }
    [[nodiscard]] double Ratio() const
    {
        if (_max == T{})
            return 0.0;
        return static_cast<double>(_current) / _max;
    }
    [[nodiscard]] bool IsEmpty() const { return (_current <= T{}); }

  private:
    T _current;
    T _max;
    void Clamp() { _current = std::clamp(_current, T{}, _max); }
};
} // namespace game::component
