#pragma once

namespace game::component {
class TextureHandle {
public:
    explicit TextureHandle(int id);
    ~TextureHandle();
    [[nodiscard]] int Id() const;
    TextureHandle(TextureHandle&& other) noexcept;
    TextureHandle &operator=(TextureHandle &&other) noexcept;
    TextureHandle(const TextureHandle &) = delete;
    TextureHandle &operator=(const TextureHandle &) = delete;
private:
    static constexpr int kNoTexture = -1;
    int _id;
};
} // namespace game::component
