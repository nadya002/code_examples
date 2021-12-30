#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <iostream>
#include <stdexcept>

// https://en.cppreference.com/w/cpp/memory/shared_ptr
struct ControlBlockBase {
    virtual ~ControlBlockBase() = default;
    virtual void OnZeroStrong() = 0;
    size_t strong = 0;
    size_t weak = 0;
};

template <typename T>
class ControlBlockPointer : public ControlBlockBase {
public:
    ControlBlockPointer(T* ptr) : ptr_(ptr) {
    }

    ~ControlBlockPointer() override {
        // if(ptr_) {
        //     delete ptr_;
        // }
        // ptr_ = nullptr;
    }
    void OnZeroStrong() override {
        std::cout << "OneZero\n";
        if (ptr_) {
            std::cout << "OneZero2\n";
            delete ptr_;
        }
        ptr_ = nullptr;
    }

    T* ptr_ = nullptr;
};

template <typename T>
struct ControlBlockMakeShared : public ControlBlockBase {

    template <typename... Args>
    ControlBlockMakeShared(Args&&... args) noexcept {
        new (&holder_) T(std::forward<Args>(args)...);
    }

    T* Get() {
        return reinterpret_cast<T*>(&holder_);
    }
    void OnZeroStrong() override {
        reinterpret_cast<T*>(&holder_)->~T();
    }
    ~ControlBlockMakeShared() {
        // reinterpret_cast<T*>(&holder_)->~T();
    }

    std::aligned_storage_t<sizeof(T), alignof(T)> holder_;
};

template <typename T>
class SharedPtr {
private:
    T* ptr_ = nullptr;
    ControlBlockBase* block_ = nullptr;

    template <typename U, typename... Args>
    friend SharedPtr<U> MakeShared(Args&&... args);

    template <typename U>
    friend class SharedPtr;

    template <typename U>
    friend class WeakPtr;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() {
        std::cout << "empty\n";
    }

    SharedPtr(std::nullptr_t) {
    }

    template <typename U>
    explicit SharedPtr(U* ptr) : ptr_(ptr) {
        std::cout << "*ptr\n";
        block_ = new ControlBlockPointer<U>(ptr);
        (block_->strong) = 1;
    }

    explicit SharedPtr(T* ptr) : ptr_(ptr) {
        std::cout << "*ptr\n";
        block_ = new ControlBlockPointer<T>(ptr_);
        (block_->strong) = 1;
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) : ptr_(other.ptr_), block_(other.block_) {
        std::cout << "const_y\n";
        if (block_) {
            (block_->strong)++;
        }
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) : ptr_(other.ptr_), block_(other.block_) {
        std::cout << "move_y\n";
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }

    SharedPtr(const SharedPtr& other) : ptr_(other.ptr_), block_(other.block_) {
        std::cout << "const\n";
        if (block_) {
            (block_->strong)++;
            std::cout << (block_->strong) << "\n";
        }
    }

    SharedPtr(SharedPtr&& other) : ptr_(other.ptr_), block_(other.block_) {
        std::cout << "move\n";
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : ptr_(ptr) {
        std::cout << "zzzz\n";
        block_ = other.block_;
        if (block_) {
            (block_->strong)++;
        }
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    // explicit SharedPtr(const WeakPtr & other) {
    //     ptr_ = other.ptr_;
    //     block_ = other.block_;
    //     if(block_){
    //         (block_->strong)++;
    //     }
    // }

    // template<typename Y>
    // explicit SharedPtr(const WeakPtr<Y> & other) {
    //     ptr_ = other.ptr_;
    //     block_ = other.block_;
    //     if(block_){
    //         (block_->strong)++;
    //     }
    // }

    template <typename Y>
    explicit SharedPtr(const WeakPtr<Y>& other) {
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (!block_ || block_->strong == 0) {
            throw BadWeakPtr();
            ptr_ = nullptr;
        }
        if (block_) {
            (block_->strong)++;
        }
    }

    explicit SharedPtr(const WeakPtr<T>& other) {
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (!block_ || block_->strong == 0) {
            throw BadWeakPtr();
            ptr_ = nullptr;
        }
        if (block_) {
            (block_->strong)++;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s
    template <typename Y>
    SharedPtr& operator=(const SharedPtr<Y>& other) {
        std::cout << "=const_y\n";
        if (static_cast<const void*>(this) == static_cast<const void*>(&other)) {
            return *this;
        }
        if (block_) {
            (block_->strong)--;
            if (block_->strong == 0) {
                block_->OnZeroStrong();
                if (block_->strong + block_->weak == 0) {
                    delete block_;
                }
            }
        }
        ptr_ = static_cast<T*>(other.ptr_);
        block_ = other.block_;
        if (block_) {
            (block_->strong)++;
        }
        return *this;
    }

    template <typename Y>
    SharedPtr& operator=(SharedPtr<Y>&& other) {
        std::cout << "=move_y\n";
        if (static_cast<void*>(this) == static_cast<void*>(&other)) {
            return *this;
        }
        if (block_) {
            (block_->strong)--;
            if (block_->strong == 0) {
                block_->OnZeroStrong();
                if (block_->strong + block_->weak == 0) {
                    delete block_;
                }
            }
        }
        ptr_ = static_cast<T*>(other.ptr_);
        block_ = other.block_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    SharedPtr& operator=(const SharedPtr& other) {
        std::cout << "=const&\n";
        if (static_cast<const void*>(this) == static_cast<const void*>(&other)) {
            return *this;
        }
        if (block_) {
            (block_->strong)--;
            if (block_->strong == 0) {
                block_->OnZeroStrong();
                if (block_->strong + block_->weak == 0) {
                    delete block_;
                }
            }
        }
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_) {
            (block_->strong)++;
        }
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        std::cout << "=move\n";
        if (static_cast<void*>(this) == static_cast<void*>(&other)) {
            return *this;
        }
        if (block_) {
            (block_->strong)--;
            if (block_->strong == 0) {
                block_->OnZeroStrong();
                if (block_->strong + block_->weak == 0) {
                    delete block_;
                }
            }
        }
        ptr_ = other.ptr_;
        block_ = other.block_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        if (block_) {
            std::cout << " AAA " << (block_->strong) << "\n";
            (block_->strong)--;
            if ((block_->strong) == 0) {
                block_->OnZeroStrong();
                if (block_->strong + block_->weak == 0) {
                    delete block_;
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            (block_->strong)--;
            if (block_->strong == 0) {
                block_->OnZeroStrong();
                if (block_->strong + block_->weak == 0) {
                    delete block_;
                }
            }
        }
        ptr_ = nullptr;
        block_ = nullptr;
        // block_ = new ControlBlockPointer<T>(nullptr);
        //(block_->strong)++;
        // ptr = ptr_;
        // count = new size_t(1);
    }
    // void Reset(T* ptr) {
    //     if(block_){
    //         (block_->strong)--;
    //         if((block_->strong) == 0) {
    //             block_->OnZeroStrong();
    //             delete block_;
    //             block_ = nullptr;
    //         }
    //     }
    //     ptr_ = ptr;
    //     block_ = new ControlBlockPointer<T>(ptr_);
    //     (block_->strong) = 1;
    // }

    template <typename U>
    void Reset(U* ptr) {
        std::cout << "WEWE\n";
        if (block_) {
            (block_->strong)--;
            if ((block_->strong) == 0) {
                block_->OnZeroStrong();
                if (block_->strong + block_->weak == 0) {
                    delete block_;
                }
            }
        }
        ptr_ = (ptr);
        block_ = new ControlBlockPointer<U>(ptr);
        (block_->strong) = 1;
    }

    void Swap(SharedPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const noexcept {
        return ptr_;
    }
    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return ptr_;
    }
    size_t UseCount() const {
        if (block_) {
            return block_->strong;
        }
        return 0;
    }

    explicit operator bool() const noexcept {
        if (ptr_ == nullptr) {
            return false;
        } else {
            return true;
        }
    }
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right);

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    // std::cout << "kollok\n";
    SharedPtr<T> a;
    // a.ptr_ = std::forward<T>(args);
    ControlBlockMakeShared<T>* b = new ControlBlockMakeShared<T>(std::forward<Args>(args)...);
    a.block_ = b;
    (a.block_)->strong++;
    a.ptr_ = b->Get();
    return a;
}

// Look for usage examples in tests
// template <typename T>
// class EnableSharedFromThis {
// public:
//     SharedPtr<T> SharedFromThis();
//     SharedPtr<const T> SharedFromThis() const;

//     WeakPtr<T> WeakFromThis() noexcept;
//     WeakPtr<const T> WeakFromThis() const noexcept;
// };
