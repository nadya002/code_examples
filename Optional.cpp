#include <memory>
struct BadOptionalAccess {
};

template <typename T>
class Optional {
private:
    alignas(T) unsigned char data[sizeof(T)];
    bool defined = false;

public:
    Optional() = default;
    Optional(const T& elem)
    {
        new (data) T(elem);
        defined = true;
    }
    Optional(T&& elem)
    {
        new (data) T(std::move(elem));
        defined = true;
    }
    Optional(const Optional& other)
    {
        if (other.defined == true) {
            new (data) T(other.value());
            defined = true;
        }
    }
    Optional& operator=(const Optional& other)
    {
        if (!other.defined) {
            reset();
        } else {
            if (defined) {
                value() = other.value();
            } else {
                new (data) T(other.value());
                defined = true;
            }
        }
        return *this;
    }
    Optional& operator=(const T& elem)
    {
        if (!defined) {
            new (data) T(elem);
            defined = true;
        } else {
            value() = elem;
        }
        return *this;
    }
    Optional& operator=(T&& elem)
    {
        if (!defined) {
            new (data) T(std::move(elem));
            defined = true;
        } else {
            value() = std::move(elem);
        }
        return *this;
    }

    bool has_value() const
    {
        return defined;
    }

    T& operator*()
    {
        if (defined)
            return *reinterpret_cast<T*>(data);
        else
            throw BadOptionalAccess();
    }
    const T& operator*() const
    {
        if (defined)
            return *reinterpret_cast<const T*>(data);
        else
            throw BadOptionalAccess();
    }

    T* operator->()
    {
        if (defined)
            return reinterpret_cast<T*>(data);
        else
            throw BadOptionalAccess();
    }
    const T* operator->() const
    {
        if (defined)
            return reinterpret_cast<const T*>(data);
        else
            throw BadOptionalAccess();
    }
    T& value()
    {
        if (defined)
            return *reinterpret_cast<T*>(data);
        else
            throw BadOptionalAccess();
    }
    const T& value() const
    {
        if (defined)
            return *reinterpret_cast<const T*>(data);
        else
            throw BadOptionalAccess();
    }

    void reset()
    {
        if (defined) {
            reinterpret_cast<T*>(data)->~T();
        }
        defined = false;
    }

    ~Optional()
    {
        reset();
    }
};