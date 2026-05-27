#pragma once

#include <compare>

namespace game::component {
class Seconds {
public:
    constexpr explicit Seconds(float value = 0.f) : _value(value) {};
    [[nodiscard]] constexpr float Value() const { return _value; };
    [[nodiscard]] friend constexpr Seconds operator+(Seconds leftMember, Seconds rightMember)
    {
        return Seconds{leftMember._value + rightMember._value};
    }
    [[nodiscard]] friend constexpr Seconds operator-(Seconds leftMember, Seconds rightMember)
    {
        return Seconds{leftMember._value - rightMember._value};
    }
    [[nodiscard]] constexpr std::partial_ordering operator<=>(const Seconds &) const = default;
private:
    float _value;
};
}
