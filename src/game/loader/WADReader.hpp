#pragma once

#include "game/loader/LumpsData.hpp"
#include "utils/FileReader/FileReader.hpp"
#include <cstdint>

namespace game::loader {
class WadLoadError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
struct LumpEntry {
    std::int32_t filePos = 0;
    std::int32_t size = 0;
    std::string name;
};
struct WadInfo {
    std::string magic;
    int32_t nb_lumps = 0;
    int32_t offset = 0;
    std::vector<LumpEntry> lumps;
};
class WADReader : public utils::FileReader::FileReader {
  public:
    using utils::FileReader::FileReader::FileReader;
    void GetHeader();
    void GetWADDirectory();
    void GetLumpsData();
    [[nodiscard]] LumpEntry GetLump(int32_t index);
    [[nodiscard]] LumpData GetLumpData() const;

  private:
    WadInfo _wadInfos;
    LumpData _lumpData;
    static constexpr size_t _magicSize = 4;
    static constexpr size_t _nameSize = 8;
    static constexpr int32_t _headerEnd = 12;

    // Helpers
    [[nodiscard]] std::string GetName();
    [[nodiscard]] glm::u8vec4 PaletteColor(std::uint8_t index) const;
    [[nodiscard]] static bool IsLevelMarker(const std::string &name);

    // Level aggregate + sub-lumps
    void ParseLevel(int32_t &i);
    void ParseThings(const LumpEntry &entry, Level &level);
    void ParseLinedefs(const LumpEntry &entry, Level &level);
    void ParseSidedefs(const LumpEntry &entry, Level &level);
    void ParseVertexes(const LumpEntry &entry, Level &level);
    void ParseSegs(const LumpEntry &entry, Level &level);
    void ParseSubSectors(const LumpEntry &entry, Level &level);
    void ParseNodes(const LumpEntry &entry, Level &level);
    void ParseSectors(const LumpEntry &entry, Level &level);
    void ParseReject(const LumpEntry &entry, Level &level);
    void ParseBlockmap(const LumpEntry &entry, Level &level);

    // Global lumps
    void ParsePalettes(const LumpEntry &entry);
    void ParseColormaps(const LumpEntry &entry);
    void ParsePnames(const LumpEntry &entry);
    void ParseTextures(const LumpEntry &entry);

    // Graphics
    void ParseFlat(const LumpEntry &entry);
    void ParsePicture(const LumpEntry &entry);
};
} // namespace game::loader
