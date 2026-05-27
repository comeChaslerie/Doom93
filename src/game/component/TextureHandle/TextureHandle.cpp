#include "game/component/TextureHandle/TextureHandle.hpp"
#include <iostream>

using namespace game::component;

TextureHandle::TextureHandle(int id) : _id(id) { std::cout << "[alloc] texture #" << _id << "\n"; }

TextureHandle::~TextureHandle()
{
    if (_id != kNoTexture)
        std::cout << "[free] texture #" << _id << "\n";
}

TextureHandle::TextureHandle(TextureHandle &&other) noexcept : _id(other._id) { other._id = kNoTexture; }

TextureHandle &TextureHandle::operator=(TextureHandle &&other) noexcept
{
    if (this != &other)
    {
        if (_id != kNoTexture)
            std::cout << "[free] texture #" << _id << "\n";
        _id = other._id;
        other._id = kNoTexture;
    }
    return *this;
}

int TextureHandle::Id() const { return _id; }
