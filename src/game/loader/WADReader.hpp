#pragma once

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
    [[nodiscard]] WadInfo GetWadInfos() const;

  private:
    WadInfo _wadInfos;
    static constexpr size_t _magicSize = 4;
    static constexpr size_t _nameSize = 8;
    static constexpr int32_t _headerEnd = 12;
};
} // namespace game::loader
