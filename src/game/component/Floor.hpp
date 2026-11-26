#pragma once

namespace game::component {
/// @brief Floor under a body, as Doom computes it from the sectors under the body's box (mobj->floorz). Set each tic by
///   game::system::ApplyStepUp to the floor of where the body ends its move. game::system::ApplyGravity reads it on the
///   next tic, to know whether the body ended the last one on its floor.
struct Floor {
    /// @brief Height of the floor, in map units.
    float height = 0.f;
};
} // namespace game::component
