#pragma once

#include "game/loader/WadLoader.hpp"
#include "utils/FileReader/FileReader.hpp"

namespace utils::FileReader {
void GetWADDirectory(FileReader &file, game::loader::WadInfo &wadInfos);
void GetHeader(FileReader &file, game::loader::WadInfo &wadInfos);
void GetWADInfos(FileReader &file, game::loader::WadInfo &wadInfos);
constexpr size_t magicSize = 4;
constexpr size_t nameSize = 8;
constexpr int32_t headerEnd = 12;
} // namespace utils::FileReader
