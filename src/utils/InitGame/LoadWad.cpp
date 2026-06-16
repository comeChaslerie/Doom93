#include "utils/InitGame/LoadWad.hpp"
#include "game/loader/WadLoader.hpp"

/// @brief Try to load WAD file
/// @return true, or false if an error occurs and logs an error
bool utils::InitGame::LoadWad(Engine::Core &core, const std::string &path)
{
    try
    {
        core.RegisterResource(game::loader::WadLoader(path));
        return true;
    }
    catch (const std::exception &error)
    {
        Log::Error(fmt::format("Loading of '{}' failed : {}", path, error.what()));
        return false;
    }
}
