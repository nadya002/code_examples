#include <algorithm>
#include <cstddef>
#include <memory>
#include <tuple>

template <typename T, class Deleter = std::default_delete<T>>
class UniquePtr {
private:
    std::tuple<T*, Deleter> data = { nullptr, Deleter() };

public:
    UniquePtr() noexcept {};
    UniquePtr(T* _ptr) noexcept
    {
        std::get<0>(data) = _ptr;
    }
    UniquePtr(T* _ptr, Deleter Deleter_) noexcept
    {
        std::get<0>(data) = _ptr;
        std::get<1>(data) = Deleter_;
    }
    UniquePtr(UniquePtr& other) = delete;
    UniquePtr(UniquePtr&& other) noexcept
    {
        std::swap(std::get<0>(data), std::get<0>(other.data));
    }
    UniquePtr operator=(UniquePtr& other) = delete;
    UniquePtr& operator=(UniquePtr&& other) noexcept
    {
        std::swap(std::get<0>(data), std::get<0>(other.data));
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t a) noexcept
    {
        if (std::get<0>(data) != nullptr) {
            std::get<1>(data)(std::get<0>(data));
        }
        std::get<0>(data) = a;
        return *this;
    }
    const T& operator*() const
    {
        return *std::get<0>(data);
    }
    T* operator->() const noexcept
    {
        return std::get<0>(data);
    }
    T* release() noexcept
    {
        T* a = std::get<0>(data);
        std::get<0>(data) = nullptr;
        return a;
    }
    void reset(T* ptr_) noexcept
    {
        if (std::get<0>(data) != nullptr) {
            std::get<1>(data)(std::get<0>(data));
        }
        std::get<0>(data) = ptr_;
    }
    void swap(UniquePtr& other) noexcept
    {
        std::swap(std::get<0>(data), std::get<0>(other.data));
    }
    T* get() const noexcept
    {
        return std::get<0>(data);
    }
    explicit operator bool() const noexcept
    {
        if (std::get<0>(data) == nullptr) {
            return false;
        } else {
            return true;
        }
    }
    const Deleter& get_deleter() const noexcept
    {
        return std::get<1>;
    }
    Deleter& get_deleter() noexcept
    {
        return std::get<1>;
    }
    ~UniquePtr()
    {
        if (std::get<0>(data) != nullptr) {
            std::get<1>(data)(std::get<0>(data));
        }
    }
};