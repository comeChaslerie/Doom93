#include "game/component/Seconds/Seconds.hpp"
#include <gtest/gtest.h>

using game::component::Seconds;

TEST(Seconds, Addition)
{
    EXPECT_FLOAT_EQ((Seconds{1.f} + Seconds{2.f}).Value(), 3.f);
    EXPECT_FLOAT_EQ((Seconds{0.f} + Seconds{0.f}).Value(), 0.f);
}

TEST(Seconds, Subtraction)
{
    EXPECT_FLOAT_EQ((Seconds{5.f} - Seconds{2.f}).Value(), 3.f);
    EXPECT_FLOAT_EQ((Seconds{2.f} - Seconds{5.f}).Value(), -3.f); // pas de borne : Seconds est neutre
}

TEST(Seconds, ComparisonsFromSpaceship)
{
    // les 6 operateurs sont generes par le seul operator<=> = default
    EXPECT_TRUE(Seconds{1.f} < Seconds{2.f});
    EXPECT_TRUE(Seconds{2.f} <= Seconds{2.f});
    EXPECT_TRUE(Seconds{2.f} == Seconds{2.f});
    EXPECT_TRUE(Seconds{3.f} != Seconds{2.f});
    EXPECT_TRUE(Seconds{3.f} > Seconds{2.f});
    EXPECT_TRUE(Seconds{2.f} >= Seconds{2.f});

    EXPECT_FALSE(Seconds{2.f} < Seconds{1.f});
    EXPECT_FALSE(Seconds{1.f} == Seconds{2.f});
}

// Preuve que Seconds est un type litteral evalue A LA COMPILATION :
// si le ctor / + / <=> n'etaient pas constexpr, ces lignes ne compileraient pas.
static_assert((Seconds{1.f} + Seconds{1.f}).Value() == 2.f, "Seconds::operator+ doit etre constexpr");
static_assert((Seconds{5.f} - Seconds{2.f}).Value() == 3.f, "Seconds::operator- doit etre constexpr");
static_assert(Seconds{1.f} < Seconds{2.f}, "Seconds::operator<=> doit etre constexpr");
