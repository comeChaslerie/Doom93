#include "game/system/Movement/UpdateKeysState.hpp"
#include "game/ressources/Keys/Keys.hpp"
#include "resource/InputManager.hpp"

void game::system::UpdateKeysState(Engine::Core &core)
{
    auto &keys = core.GetResource<game::ressource::Keys>();
    auto &input = core.GetResource<Input::Resource::InputManager>();

    for (auto &[type, key] : keys.GetKeys())
    {
        if (input.IsKeyPressed(key.keyCode))
            key.state = true;
        else
            key.state = false;
    }
}
