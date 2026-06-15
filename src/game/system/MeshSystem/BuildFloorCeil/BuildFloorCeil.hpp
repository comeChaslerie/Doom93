#pragma once

#include "component/Mesh.hpp"
#include "game/loader/LumpsData.hpp"

namespace game::system {
Object::Component::Mesh BuildFloorCeil(const game::loader::Level &level);
} // namespace game::system
