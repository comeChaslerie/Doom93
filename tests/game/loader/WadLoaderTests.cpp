#include "game/loader/LumpsData.hpp"
#include "game/loader/WadLoader.hpp"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <gtest/gtest.h>

using game::loader::Level;
using game::loader::LumpData;

namespace {
constexpr const char *WAD_PATH = "freedoom1.wad";

// Constantes du format / de la fixture freedoom1.wad (verifiees a la main).
constexpr std::size_t EXPECTED_PALETTES = 14;  // PLAYPAL : 14 palettes standard
constexpr std::size_t EXPECTED_COLORMAPS = 34; // COLORMAP : 34 tables standard
constexpr std::size_t EXPECTED_LEVELS = 36;    // 4 episodes x 9 maps

// Charge le WAD une seule fois pour toute la suite. Si le fichier est absent
// (environnement sans la fixture), les tests sont skippes au lieu d'echouer.
class WadLoaderTest : public ::testing::Test {
  protected:
    static LumpData _wad;
    static bool _loaded;

    static void SetUpTestSuite()
    {
        if (std::filesystem::exists(WAD_PATH))
        {
            _wad = game::loader::WadLoader(WAD_PATH);
            _loaded = true;
        }
    }

    static const LumpData &Wad() { return _wad; }

    void SetUp() override
    {
        if (!_loaded)
            GTEST_SKIP() << "freedoom1.wad introuvable (cwd=" << std::filesystem::current_path().string() << ")";
    }
};
LumpData WadLoaderTest::_wad;
bool WadLoaderTest::_loaded = false;
} // namespace

TEST_F(WadLoaderTest, ParsesAllPalettes) { EXPECT_EQ(Wad().palettes.size(), EXPECTED_PALETTES); }

TEST_F(WadLoaderTest, PalettesAreOpaque)
{
    for (const auto &palette : Wad().palettes)
        for (const auto &color : palette.colors)
            EXPECT_EQ(color.a, 255); // alpha force a 255 au parsing
}

TEST_F(WadLoaderTest, ParsesAllColormaps) { EXPECT_EQ(Wad().colormaps.size(), EXPECTED_COLORMAPS); }

TEST_F(WadLoaderTest, ParsesAllLevels) { EXPECT_EQ(Wad().levels.size(), EXPECTED_LEVELS); }

TEST_F(WadLoaderTest, ParsesGlobalGraphics)
{
    EXPECT_FALSE(Wad().pnames.empty());
    EXPECT_FALSE(Wad().textures.empty());
    EXPECT_FALSE(Wad().flats.empty());
    EXPECT_FALSE(Wad().pictures.empty());
}

TEST_F(WadLoaderTest, FirstLevelHasGeometry)
{
    ASSERT_FALSE(Wad().levels.empty());
    const Level &level = Wad().levels.front();

    EXPECT_FALSE(level.name.empty());
    EXPECT_FALSE(level.vertexes.empty());
    EXPECT_FALSE(level.linedefs.empty());
    EXPECT_FALSE(level.sidedefs.empty());
    EXPECT_FALSE(level.sectors.empty());
    EXPECT_FALSE(level.things.empty());
}

TEST_F(WadLoaderTest, LevelE1M1IsPresentAndComplete)
{
    const auto &levels = Wad().levels;
    const auto it = std::ranges::find_if(levels.begin(), levels.end(), [](const Level &lvl) { return lvl.name == "E1M1"; });

    ASSERT_NE(it, levels.end()) << "E1M1 absent du WAD";
    EXPECT_FALSE(it->vertexes.empty());
    EXPECT_FALSE(it->linedefs.empty());
    EXPECT_FALSE(it->sectors.empty());
}
