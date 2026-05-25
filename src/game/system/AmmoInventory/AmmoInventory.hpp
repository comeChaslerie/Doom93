#pragma once

#include "game/component/Ammo/Ammo.hpp"
#include "game/component/Ammo/AmmoType.hpp"
#include <optional>

namespace game::system {
[[nodiscard]] bool TryConsume(component::Ammo &ammo, component::AmmoType type, int n);
void Add(component::Ammo &ammo, component::AmmoType type, int n);
[[nodiscard]] std::optional<int> Peek(const component::Ammo &ammo, component::AmmoType type);
[[nodiscard]] int TotalRounds(const component::Ammo &ammo);
} // namespace game::system
