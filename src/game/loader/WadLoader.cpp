#include "game/loader/WadLoader.hpp"
#include "game/loader/WADReader.hpp"

game::loader::LumpData game::loader::WadLoader(const std::string &filepath)
{
    WADReader wadReader(filepath);

    wadReader.CheckFileIntegrity(0, game::loader::WADReader::CheckState::StreamOnly);
    wadReader.GetHeader();
    wadReader.GetWADDirectory();
    wadReader.GetLumpsData();
    return wadReader.GetLumpData();
}
