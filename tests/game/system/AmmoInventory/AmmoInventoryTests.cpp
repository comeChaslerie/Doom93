#include "game/component/Ammo/Ammo.hpp"
#include "game/component/Ammo/AmmoType.hpp"
#include "game/system/AmmoInventory/AmmoInventory.hpp"
#include <gtest/gtest.h>

using namespace game::component;

TEST(AmmoInventory, AddAndPeek)
{
    Ammo ammo;
    std::optional<int> nb_bullets = 0;
    std::optional<int> nb_rockets = 0;

    game::system::Add(ammo, AmmoType::Bullets, 10);
    nb_bullets = game::system::Peek(ammo, AmmoType::Bullets);
    nb_rockets = game::system::Peek(ammo, AmmoType::Rockets);
    EXPECT_TRUE(nb_bullets);
    EXPECT_EQ(nb_bullets, 10);
    EXPECT_FALSE(nb_rockets);
    EXPECT_FALSE(game::system::TryConsume(ammo, AmmoType::Rockets, 10));
    EXPECT_FALSE(game::system::TryConsume(ammo, AmmoType::Rockets, 0));
    EXPECT_FALSE(game::system::TryConsume(ammo, game::component::AmmoType::Bullets, 50));
    EXPECT_TRUE(game::system::TryConsume(ammo, game::component::AmmoType::Bullets, 5));
    EXPECT_EQ(game::system::TotalRounds(ammo), 5);
}
