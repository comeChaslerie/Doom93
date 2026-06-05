#include "game/loader/GetWADInfos.hpp"
#include <cstddef>
#include <cstdlib>
#include "utils/FileReader/FileReader.hpp"

void utils::FileReader::GetHeader(FileReader &file, game::loader::WadInfo &wadInfos)
{
    wadInfos.magic = file.GetString(magicSize);
    file.CheckFileIntegrity(magicSize, true);
    if (wadInfos.magic != "IWAD" && wadInfos.magic != "PWAD")
        throw game::loader::WadLoadError("bad WAD magic " + wadInfos.magic);
    wadInfos.nb_lumps = file.GetInt32();
    file.CheckFileIntegrity(file._int32Size, true);
    wadInfos.offset = file.GetInt32();
    file.CheckFileIntegrity(file._int32Size, true);
}

void utils::FileReader::GetWADDirectory(FileReader &file, game::loader::WadInfo &wadInfos)
{
    game::loader::LumpEntry lump;

    if (wadInfos.offset < 0 || wadInfos.nb_lumps < headerEnd)
        throw game::loader::WadLoadError("bad header infos");
    file.Seek(wadInfos.offset);
    file.CheckFileIntegrity(0, true);
    wadInfos.lumps.reserve(wadInfos.nb_lumps);
    for (int32_t index = 0; index < wadInfos.nb_lumps; index++)
    {
        file.CheckFileIntegrity(0, true);
        lump.filePos = file.GetInt32();
        file.CheckFileIntegrity(file._int32Size, true);
        lump.size = file.GetInt32();
        file.CheckFileIntegrity(file._int32Size, true);
        lump.name = file.GetString(nameSize);
        file.CheckFileIntegrity(nameSize, false);
        wadInfos.lumps.push_back(lump);
    }
}

void utils::FileReader::GetWADInfos(FileReader &file, game::loader::WadInfo &wadInfos)
{
    GetHeader(file, wadInfos);
    GetWADDirectory(file, wadInfos);
}
