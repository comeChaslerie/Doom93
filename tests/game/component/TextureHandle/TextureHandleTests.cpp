#include "game/component/TextureHandle/TextureHandle.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

using namespace game::component;

// compte combien de fois `needle` apparait dans `haystack`
static int CountSubstr(const std::string &haystack, const std::string &needle)
{
    int count = 0;
    for (std::string::size_type pos = haystack.find(needle); pos != std::string::npos;
         pos = haystack.find(needle, pos + needle.size()))
        ++count;
    return count;
}

TEST(TextureHandle, MoveCtorTransfersOwnership)
{
    TextureHandle a(42);
    TextureHandle b(std::move(a));
    EXPECT_EQ(b.Id(), 42);
    EXPECT_EQ(a.Id(), -1);
}

TEST(TextureHandle, MoveAssignTransfersOwnership)
{
    TextureHandle a(1);
    TextureHandle b(2);
    b = std::move(a);
    EXPECT_EQ(b.Id(), 1);
    EXPECT_EQ(a.Id(), -1);
}

TEST(TextureHandle, MovedFromFreesNothing_NoDoubleFree)
{
    std::ostringstream captured;
    std::streambuf *old = std::cout.rdbuf(captured.rdbuf()); // détourne cout -> captured
    {
        TextureHandle a(7);
        TextureHandle b(std::move(a));
    }
    std::cout.rdbuf(old); // restaure cout AVANT toute assertion
    const std::string out = captured.str();
    EXPECT_EQ(CountSubstr(out, "[free] texture #"), 1);  // exactement 1 free -> pas de double-free
    EXPECT_EQ(CountSubstr(out, "[free] texture #7"), 1); // et c'est bien #7 qui est libere
}

TEST(TextureHandle, SharedPtrSharesOneTexture)
{
    auto tex = std::make_shared<TextureHandle>(7);
    EXPECT_EQ(tex.use_count(), 1);
    {
        auto entityA = tex;
        auto entityB = tex;
        EXPECT_EQ(tex.use_count(), 3);
        EXPECT_EQ(entityB->Id(), 7);
    }
    EXPECT_EQ(tex.use_count(), 1);
}

TEST(TextureHandle, UniquePtrMoveTransfersOwnership)
{
    auto owner = std::make_unique<TextureHandle>(99);
    std::unique_ptr<TextureHandle> newOwner = std::move(owner);
    EXPECT_EQ(owner.get(), nullptr);
    EXPECT_EQ(newOwner->Id(), 99);
}
