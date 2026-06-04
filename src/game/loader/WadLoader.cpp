#include "game/loader/WadLoader.hpp"
#include "utils/FileReader/FileReader.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <string>
#include <vector>

namespace {
constexpr size_t magicSize = 4;
constexpr size_t nameSize = 8;
constexpr int32_t directoryStart = 12;
}   // namespace

using namespace utils::FileReader;

void GetHeader(std::ifstream &file, game::loader::WadInfo &wadInfos)
{
    wadInfos.magic = GetString(file, magicSize);
    checkFileIntegrity(file, magicSize, true);
    if (wadInfos.magic != "IWAD" && wadInfos.magic != "PWAD")
        throw game::loader::WadLoadError("bad WAD magic " + wadInfos.magic);
    wadInfos.nb_lumps = GetInt32(file);
    checkFileIntegrity(file, int32Size, true);
    wadInfos.offset = GetInt32(file);
    checkFileIntegrity(file, int32Size, true);
}

void GetWADDirectory(std::ifstream &file, game::loader::WadInfo &wadInfos)
{
    game::loader::LumpEntry lump;

    if (wadInfos.offset < 0 || wadInfos.nb_lumps < directoryStart)
        throw game::loader::WadLoadError("bad header infos");
    file.seekg(wadInfos.offset);
    checkFileIntegrity(file, 0, true);
    wadInfos.lumps.reserve(wadInfos.nb_lumps);
    for (int32_t index = 0; index < wadInfos.nb_lumps; index++) {
        checkFileIntegrity(file, 0, true);
        lump.filePos = GetInt32(file);
        checkFileIntegrity(file, int32Size, true);
        lump.size = GetInt32(file);
        checkFileIntegrity(file, int32Size, true);
        lump.name = GetString(file, nameSize);
        checkFileIntegrity(file, nameSize, false);
        wadInfos.lumps.push_back(lump);
    }
}

game::loader::WadInfo game::loader::WadLoader(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary);
    WadInfo wadInfos;

    if (!file)
        throw WadLoadError("Cannot open WAD" + filepath);
    GetHeader(file, wadInfos);
    GetWADDirectory(file, wadInfos);
    return wadInfos;
}
