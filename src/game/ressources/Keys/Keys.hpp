#pragma once

#include "game/component/Keys/Key.hpp"
#include "game/component/Keys/KeysType.hpp"
#include <unordered_map>

namespace game::ressource {
class Keys {
  public:
    explicit Keys() {}
    void SetKeyCode(component::KeysType key, int keyCode) { _keys[key].keyCode = keyCode; }
    void SetKeyState(component::KeysType key, bool state) { _keys[key].state = state; }
    [[nodiscard]] int GetKeyCode(component::KeysType key) { return _keys.at(key).keyCode; }
    [[nodiscard]] bool GetKeyState(component::KeysType key) { return _keys.at(key).state; }
    std::unordered_map<component::KeysType, component::Key> &GetKeys() { return _keys; }

  private:
    std::unordered_map<component::KeysType, component::Key> _keys;
};
} // namespace game::ressource
