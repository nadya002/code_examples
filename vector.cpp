#include <algorithm>
#include <cmath>
class Vector {
private:
    struct vmem {
        size_t cap = 0;
        int* ptr = nullptr;
        vmem() { }
        vmem(const vmem& other)
        {
            *this = other;
        }
        vmem(vmem&& other)
        {
            *this = other;
        }
        vmem& operator=(const vmem& other)
        {
            resize(other.cap);
            std::copy(other.ptr, other.ptr + cap, ptr);
            return *this;
        }
        vmem& operator=(vmem&& other)
        {
            std::swap(cap, other.cap);
            std::swap(ptr, other.ptr);
            return *this;
        }
        void resize(size_t len)
        {
            if (len <= cap) {
                return;
            }
            int* ptr_ = new int[len];
            std::copy(ptr, ptr + cap, ptr_);
            std::fill(ptr_ + cap, ptr_ + len, 0);
            reset();
            ptr = ptr_;
            cap = len;
        }
        void reset()
        {
            cap = 0;
            if (ptr != nullptr) {
                delete[] ptr;
                ptr = nullptr;
            }
        }
        ~vmem()
        {
            reset();
        }
    } mem;
    size_t sz = 0;

public:
    Vector() { }
    Vector(const Vector& other)
        : sz(other.sz)
        , mem(other.mem)
    {
    }
    Vector& operator=(const Vector& other)
    {
        sz = other.sz;
        mem = other.mem;
        return *this;
    }
    void pop_back()
    {
        sz--;
    }
    void push_back(int a)
    {
        if (sz == mem.cap) {
            mem.resize(std::max<size_t>(1, 2 * sz));
        }
        mem.ptr[sz] = a;
        sz++;
    }
    size_t size() const
    {
        return sz;
    }
    int operator[](size_t i) const
    {
        return mem.ptr[i];
    }
    int& operator[](size_t i)
    {
        return mem.ptr[i];
    }
    auto begin()
    {
        return mem.ptr;
    }
    auto end()
    {
        return mem.ptr + sz;
    }
    auto begin() const
    {
        return mem.ptr;
    }
    auto end() const
    {
        return mem.ptr + sz;
    }
    ~Vector() { }
};