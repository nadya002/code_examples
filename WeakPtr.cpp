#pragma once

#include "sw_fwd.h"  // Forward declaration
#include "shared.h"
#include <stdexcept>
// https://en.cppreference.com/w/cpp/memory/weak_ptr

template <typename T>
class WeakPtr {
private:
    T* ptr_ = nullptr;
    ControlBlockBase* block_ = nullptr;

    template <typename U>
    friend class WeakPtr;

    template <typename U>
    friend class SharedPtr;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() {
    }

    WeakPtr(const WeakPtr& other) {
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_) {
            (block_->weak)++;
        }
    }

    template <typename U>
    WeakPtr(const WeakPtr<U>& other) {
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_) {
            (block_->weak)++;
        }
    }

    WeakPtr(WeakPtr&& other) {
        ptr_ = other.ptr_;
        block_ = other.block_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }

    template <typename U>
    WeakPtr(WeakPtr<U>&& other) {
        ptr_ = other.ptr_;
        block_ = other.block_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    template <typename U>
    WeakPtr(const SharedPtr<U>& other) {
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_) {
            (block_->weak)++;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) {
        if (static_cast<const void*>(this) == static_cast<const void*>(&other)) {
            return *this;
        }
        if (block_) {
            (block_->weak)--;
            if (block_->strong + block_->weak == 0) {
                delete block_;
            }
        }
        ptr_ = static_cast<T*>(other.ptr_);
        block_ = other.block_;
        if (block_) {
            (block_->weak)++;
        }
        return *this;
    }

    template <typename U>
    WeakPtr& operator=(const WeakPtr<U>& other) {
        if (static_cast<const void*>(this) == static_cast<const void*>(&other)) {
            return *this;
        }
        if (block_) {
            (block_->weak)--;
            if (block_->strong + block_->weak == 0) {
                delete block_;
            }
        }
        ptr_ = static_cast<T*>(other.ptr_);
        block_ = other.block_;
        if (block_) {
            (block_->weak)++;
        }
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
        if (static_cast<const void*>(this) == static_cast<const void*>(&other)) {
            return *this;
        }
        if (block_) {
            (block_->weak)--;
            if (block_->strong + block_->weak == 0) {
                delete block_;
            }
        }
        ptr_ = static_cast<T*>(other.ptr_);
        block_ = other.block_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    template <typename U>
    WeakPtr& operator=(WeakPtr<U>&& other) {
        if (static_cast<const void*>(this) == static_cast<const void*>(&other)) {
            return *this;
        }
        if (block_) {
            (block_->weak)--;
            if (block_->strong + block_->weak == 0) {
                delete block_;
            }
        }
        ptr_ = static_cast<T*>(other.ptr_);
        block_ = other.block_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        if (block_) {
            (block_->weak)--;
            if (block_->strong + block_->weak == 0) {
                delete block_;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            (block_->weak)--;
            if (block_->strong + block_->weak == 0) {
                delete block_;
            }
        }
        ptr_ = nullptr;
        block_ = nullptr;
    }

    void Swap(WeakPtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(other.block_, block_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    size_t UseCount() const {
        if (block_) {
            return block_->strong;
        }
        return 0;
    }
    bool Expired() const {
        if (!ptr_) {
            return !(ptr_);
        }
        if (block_->strong == 0) {
            return true;
        }
        return false;
    }

    SharedPtr<T> Lock() const {
        if (!block_ || block_->strong == 0) {
            return SharedPtr<T>(nullptr);
        }
        return SharedPtr<T>(*this);
    }
};
