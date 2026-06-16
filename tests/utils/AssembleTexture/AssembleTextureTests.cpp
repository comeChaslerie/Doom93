#include "game/loader/LumpsData.hpp"
#include "utils/AssembleTexture/AssembleTexture.hpp"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <gtest/gtest.h>

using utils::AssembleTexture::AssembleTexture;

namespace {
const glm::u8vec4 RED{255, 0, 0, 255};
const glm::u8vec4 BLANK{0, 0, 0, 0};

bool SameColor(const glm::u8vec4 &a, const glm::u8vec4 &b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

game::loader::Picture MakePicture(const std::string &name, int w, int h, glm::u8vec4 color)
{
    game::loader::Picture pic;
    pic.name = name;
    pic.width = w;
    pic.height = h;
    pic.pixels.assign(static_cast<size_t>(w) * h, color);
    return pic;
}

// Texture w x h avec un seul patch (patchIndex 0) place a `origin`.
game::loader::TextureDef MakeTextureDef(int w, int h, glm::ivec2 origin)
{
    game::loader::TextureDef def;
    def.name = "WALL";
    def.width = w;
    def.height = h;
    game::loader::TexturePatch patch;
    patch.origin = origin;
    patch.patchIndex = 0;
    def.patches.push_back(patch);
    return def;
}
} // namespace

TEST(AssembleTexture, CanvasHasTextureDimensions)
{
    auto tex = AssembleTexture(MakeTextureDef(4, 4, {1, 1}), {"PATCH0"}, {MakePicture("PATCH0", 2, 2, RED)});

    EXPECT_EQ(tex.width, 4);
    EXPECT_EQ(tex.height, 4);
    EXPECT_EQ(tex.pixels.size(), 16U);
}

TEST(AssembleTexture, PatchIsPastedAtItsOrigin)
{
    auto tex = AssembleTexture(MakeTextureDef(4, 4, {1, 1}), {"PATCH0"}, {MakePicture("PATCH0", 2, 2, RED)});

    // patch 2x2 a (1,1) -> canevas (1,1),(2,1),(1,2),(2,2) rouges
    EXPECT_TRUE(SameColor(tex.pixels[(1 * 4) + 1], RED));
    EXPECT_TRUE(SameColor(tex.pixels[(1 * 4) + 2], RED));
    EXPECT_TRUE(SameColor(tex.pixels[(2 * 4) + 1], RED));
    EXPECT_TRUE(SameColor(tex.pixels[(2 * 4) + 2], RED));
    // le coin (0,0) reste transparent
    EXPECT_TRUE(SameColor(tex.pixels[0], BLANK));
}

TEST(AssembleTexture, TransparentPixelsAreNotPasted)
{
    auto pic = MakePicture("PATCH0", 2, 2, RED);
    pic.pixels[0] = BLANK; // coin haut-gauche du patch transparent

    auto tex = AssembleTexture(MakeTextureDef(4, 4, {0, 0}), {"PATCH0"}, {pic});

    EXPECT_TRUE(SameColor(tex.pixels[0], BLANK)); // non recouvert par le pixel transparent
    EXPECT_TRUE(SameColor(tex.pixels[1], RED));   // les opaques le sont
}

TEST(AssembleTexture, OutOfBoundsPixelsAreClipped)
{
    // patch 2x2 a (3,3) dans un canevas 4x4 -> seul (3,3) tient
    auto tex = AssembleTexture(MakeTextureDef(4, 4, {3, 3}), {"PATCH0"}, {MakePicture("PATCH0", 2, 2, RED)});

    EXPECT_TRUE(SameColor(tex.pixels[(3 * 4) + 3], RED));
    int redCount = 0;
    for (const auto &p : tex.pixels)
        if (SameColor(p, RED))
            redCount++;
    EXPECT_EQ(redCount, 1); // pas de crash, un seul pixel colle
}

TEST(AssembleTexture, MissingPatchIsSkipped)
{
    // pnames[0] = "NOPE" mais aucune Picture de ce nom -> rien de colle
    auto tex = AssembleTexture(MakeTextureDef(4, 4, {1, 1}), {"NOPE"}, {MakePicture("OTHER", 2, 2, RED)});

    for (const auto &p : tex.pixels)
        EXPECT_TRUE(SameColor(p, BLANK));
}
