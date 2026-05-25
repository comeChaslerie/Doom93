#include "game/system/AmmoInventory/AmmoInventory.hpp"
#include "game/component/Ammo/Ammo.hpp"
#include "game/component/Ammo/AmmoType.hpp"
#include <optional>

using namespace game::component;

bool game::system::TryConsume(Ammo &ammo, AmmoType type, int n)
{
    auto it = ammo.stock.find(type);

    if (it == ammo.stock.end() || it->second < n)
        return false;
    it->second -= n;
    return true;
}

void game::system::Add(Ammo &ammo, AmmoType type, int n) { ammo.stock[type] += n; }

std::optional<int> game::system::Peek(const Ammo &ammo, AmmoType type)
{
    auto it = ammo.stock.find(type);

    if (it == ammo.stock.end())
        return std::nullopt;
    return it->second;
}

int game::system::TotalRounds(const Ammo &ammo)
{
    int total = 0;

    for (const auto &[type, count] : ammo.stock)
        total += count;
    return total;
}
