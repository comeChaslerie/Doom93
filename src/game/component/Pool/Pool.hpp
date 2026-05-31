#pragma once

#include <cstddef>
#include <vector>

namespace game::component {
template <typename T> class Pool {
  public:
    explicit Pool(std::size_t capacity)
    {
        _storage.reserve(capacity);
        _free.reserve(capacity);
        for (size_t index = 0; index < capacity; index++)
        {
            _storage.emplace_back(T{});
            _free.push_back(&_storage.back());
        }
    };
    T *Acquire()
    {
        T *p = nullptr;

        if (_free.empty())
            return p;
        p = _free.back();
        _free.pop_back();
        return p;
    };
    void Release(T *obj) { _free.push_back(obj); }
    [[nodiscard]] std::size_t Available() const { return _free.size(); }
    [[nodiscard]] std::size_t Capacity() const { return _storage.size(); }
    Pool(const Pool &other) = delete;
    Pool &operator=(const Pool &other) = delete;
    Pool(Pool &&other) noexcept = default;
    Pool &operator=(Pool &&other) noexcept = default;

  private:
    std::vector<T> _storage;
    std::vector<T *> _free;
};
} // namespace game::component