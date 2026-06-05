#include "game/loader/WADReader.hpp"
#include "utils/FileReader/FileReader.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdlib>

void game::loader::WADReader::GetHeader()
{
    _wadInfos.magic = GetString(_magicSize);
    CheckFileIntegrity(_magicSize, FileReader::CheckState::BytesAndStream);
    if (_wadInfos.magic != "IWAD" && _wadInfos.magic != "PWAD")
        throw game::loader::WadLoadError("bad WAD magic " + _wadInfos.magic);
    _wadInfos.nb_lumps = GetInt32();
    CheckFileIntegrity(_int32Size, FileReader::CheckState::BytesAndStream);
    _wadInfos.offset = GetInt32();
    CheckFileIntegrity(_int32Size, FileReader::CheckState::BytesAndStream);
}

void game::loader::WADReader::GetWADDirectory()
{
    game::loader::LumpEntry lump;

    if (_wadInfos.offset < 0 || _wadInfos.nb_lumps < _headerEnd)
        throw game::loader::WadLoadError("bad header infos");
    Seek(_wadInfos.offset);
    CheckFileIntegrity(0, FileReader::CheckState::StreamOnly);
    _wadInfos.lumps.reserve(_wadInfos.nb_lumps);
    for (int32_t index = 0; index < _wadInfos.nb_lumps; index++)
    {
        CheckFileIntegrity(0, FileReader::CheckState::StreamOnly);
        lump.filePos = GetInt32();
        CheckFileIntegrity(_int32Size, FileReader::CheckState::BytesAndStream);
        lump.size = GetInt32();
        CheckFileIntegrity(_int32Size, FileReader::CheckState::BytesAndStream);
        lump.name = GetString(_nameSize);
        CheckFileIntegrity(_nameSize, FileReader::CheckState::BytesOnly);
        _wadInfos.lumps.push_back(lump);
    }
}

game::loader::LumpEntry game::loader::WADReader::GetLump(int32_t index) { return _wadInfos.lumps[index]; }

void game::loader::WADReader::GetLumpsData()
{
    for (int32_t index = 0; index < _wadInfos.nb_lumps; index++)
    {
        Seek(GetLump(index).filePos);
        CheckFileIntegrity(0, CheckState::StreamOnly);
    }
}

game::loader::WadInfo game::loader::WADReader::GetWadInfos() const { return _wadInfos; }
