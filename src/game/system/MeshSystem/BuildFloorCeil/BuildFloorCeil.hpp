#pragma once

#include "component/Mesh.hpp"
#include "game/loader/LumpsData.hpp"
#include <map>

namespace game::system {
std::map<std::string, Object::Component::Mesh> BuildFloorCeil(const game::loader::Level &level);
} // namespace game::system
