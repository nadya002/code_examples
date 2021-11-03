#pragma once

#include <cstddef>  // std::nullptr_t
#include <iostream>

#include <type_traits>
#include <algorithm>
#include <utility>

template <typename T>
struct IsMove {
    static constexpr const bool kValue = false;
    // using reference = T*;
};

template <typename T>
struct IsMove<T&&> {
    static constexpr const bool kValue = true;
    // using reference = T*;
};


template <typename T, size_t I, bool = std::is_empty_v<T> && !std::is_final_v<T>>
struct CompressedPairElement {
public:
    CompressedPairElement() : val_(T()) {
    }
    template <typename U>
    CompressedPairElement(U&& v) : val_(std::forward<U>(v)) {
    }

    T& Getter() {
        return val_;
    }
    const T& Getter() const {
        return val_;
    }
private:
    T val_;
};

template <typename T, size_t I>
struct CompressedPairElement<T, I, true> : public T {
public:
    CompressedPairElement() {
    }
  
    template <typename U>
    CompressedPairElement(U&& v) {
    }

    T& Getter() {
        return *this;
    }
    const T& Getter() const {
        return *this;
    }
};

template <typename F, typename S>
class CompressedPair : private CompressedPairElement<F, 0>, private CompressedPairElement<S, 1> {
    using First = CompressedPairElement<F, 0>;
    using Second = CompressedPairElement<S, 1>;

public:
    CompressedPair() {
    }

    template <typename R, typename U>
    CompressedPair(R&& first, U&& second)
        : First(std::forward<R>(first)), Second(std::forward<U>(second)) {
    }

    F& GetFirst() {
        return (static_cast<First*>(this))->Getter();
    }

    const S& GetSecond() const {
        return (static_cast<const Second*>(this))->Getter();
    }
    const F& GetFirst() const {
        return (static_cast<const First*>(this))->Getter();
    }

    S& GetSecond() {
        return (static_cast<Second*>(this))->Getter();
    }
};

template <typename T>
struct IsArray {
    static constexpr const bool kValue = false;
    using Reference = T*;
};

template <typename T>
struct IsArray<T[]> {
    static constexpr const bool kValue = true;
    using Reference = T*;
};

template <typename T>
struct Slug {
    void operator()(T* a) {
        delete a;
    }
};

template <typename T>
struct Slug<T[]> {
    void operator()(T* a) {
        delete[] a;
    }
};

// Primary templates
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
private:
    // T* ptr_ = nullptr;
    CompressedPair<T*, Deleter> p_;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    explicit UniquePtr(T* ptr = nullptr) : p_(ptr, Deleter()) {
    }
    UniquePtr(UniquePtr& other) = delete;

    // UniquePtr(T* ptr, const Deleter& deleter) : p_(ptr, deleter) {
    // }

    template <typename del>
    UniquePtr(T* ptr, del&& deleter) : p_(ptr, std::forward<del>(deleter)) {
    }

    template <typename T1, typename Deleter2>
    UniquePtr(UniquePtr<T1, Deleter2>&& other) noexcept
        : p_(static_cast<T*>(other.Release()), std::forward<Deleter2>(other.GetDeleter())) {
    }
  

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s
    UniquePtr operator=(UniquePtr& other) = delete;

    template <typename T1, typename Deleter2>
    UniquePtr& operator=(UniquePtr<T1, Deleter2>&& other) noexcept {
        if (static_cast<void*>(this) == static_cast<void*>(&other)) {
            return *this;
        }
        if (p_.GetFirst()) {
            p_.GetSecond()(p_.GetFirst());
        }
        p_.GetFirst() = static_cast<T*>(other.Release());
        p_.GetSecond() = std::forward<Deleter2>(other.GetDeleter());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        if (p_.GetFirst()) {
            p_.GetSecond()(p_.GetFirst());
        }
        p_.GetFirst() = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (p_.GetFirst()) {
            p_.GetSecond()(p_.GetFirst());
        }
        p_.GetFirst() = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* a = p_.GetFirst();
        p_.GetFirst() = nullptr;
        return a;
    }

    void Reset(T* ptr = nullptr) noexcept {
        T* old = p_.GetFirst();
        p_.GetFirst() = ptr;
        if (old != nullptr) {
            p_.GetSecond()(old);
        }
    }

    void Swap(UniquePtr& other) noexcept {
        std::swap(p_.GetFirst(), other.p_.GetFirst());
        std::swap(p_.GetSecond(), other.p_.GetSecond());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const noexcept {
        return p_.GetFirst();
    }

    Deleter& GetDeleter() {
        return p_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return p_.GetSecond();
    }
    explicit operator bool() const {
        return p_.GetFirst();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    T& operator*() const {
        return *p_.GetFirst();
    }
    T* operator->() const {
        return p_.GetFirst();
    }
};

template <typename Deleter>
class UniquePtr<void, Deleter> {
private:
    // T* ptr_ = nullptr;
    CompressedPair<void*, Deleter> p_;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    // UniquePtr() noexcept {};
    explicit UniquePtr(void* ptr = nullptr) : p_(ptr, Deleter()) {
    }
    UniquePtr(UniquePtr& other) = delete;

    UniquePtr(void* ptr, const Deleter& deleter) : p_(ptr, deleter) {
    }

    template <typename T1>
    UniquePtr(UniquePtr<T1>&& other) noexcept
        : p_(static_cast<void*>(other.Release()), std::move(other.GetDeleter())) {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s
    UniquePtr operator=(UniquePtr& other) = delete;
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        std::swap(p_.GetFirst(), p_.GetFirst());
        std::swap(p_.GetSecond(), p_.GetSecond());
        if (other.p_.GetFirst()) {
            p_.GetSecond()(p_.GetFirst());
        }
        other.p_.GetFirst() = nullptr;
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        if (p_.GetFirst()) {
            p_.GetSecond()(p_.GetFirst());
        }
        p_.GetFirst() = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (p_.GetFirst()) {
            p_.GetSecond()(p_.GetFirst());
        }
        p_.GetFirst() = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void* Release() {
        void* a = p_.GetFirst();
        p_.GetFirst() = nullptr;
        return a;
    }

    void Reset(void* ptr = nullptr) noexcept {
        void* old = p_.GetFirst();
        p_.GetFirst() = ptr;
        if (old != nullptr) {
            p_.GetSecond()(old);
        }
    }

    void Swap(UniquePtr& other) noexcept {
        std::swap(p_.GetFirst(), other.p_.GetFirst());
        std::swap(p_.GetFirst(), other.p_.GetSecond());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    void* Get() const noexcept {
        return p_.GetFirst();
    }

    Deleter& GetDeleter() {
        return p_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return p_.GetSecond();
    }
    explicit operator bool() const {
        return p_.GetFirst();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    void* operator->() const {
        return p_.GetFirst();
    }
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
private:
    CompressedPair<T*, Deleter> p_;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    // UniquePtr() noexcept {};
    explicit UniquePtr(T* ptr = nullptr) : p_(ptr, Deleter()) {
    }

    UniquePtr(UniquePtr& other) = delete;

    UniquePtr(T* ptr, Deleter deleter) : p_(ptr, deleter) {
    }

    template <typename T1>
    UniquePtr(UniquePtr<T1>&& other) noexcept
        : p_(static_cast<void*>(other.Release()), std::move(other.GetDeleter())) {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s
    UniquePtr operator=(UniquePtr& other) = delete;
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        std::swap(p_.GetFirst(), p_.GetFirst());
        std::swap(p_.GetSecond(), p_.GetSecond());
        if (other.p_.GetFirst()) {
            p_.GetSecond()(p_.GetFirst());
        }
        other.p_.GetFirst() = nullptr;
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        if (p_.GetFirst()) {
            p_.GetSecond()(p_.GetFirst());
        }
        p_.GetFirst() = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (p_.GetFirst()) {
            p_.GetSecond()(p_.GetFirst());
        }
        p_.GetFirst() = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* a = p_.GetFirst();
        p_.GetFirst() = nullptr;
        return a;
    }

    void Reset(T* ptr = nullptr) noexcept {
        T* old = p_.GetFirst();
        p_.GetFirst() = ptr;
        if (old != nullptr) {
            p_.GetSecond()(old);
        }
    }

    void Swap(UniquePtr& other) noexcept {
        std::swap(p_.GetFirst(), other.p_.GetFirst());
        std::swap(p_.GetFirst(), other.p_.GetSecond());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const noexcept {
        return p_.GetFirst();
    }

    Deleter& GetDeleter() {
        return p_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return p_.GetSecond();
    }

    explicit operator bool() const {
        return p_.GetFirst();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators
    T& operator*() const {
        return *p_.GetFirst();
    }

    T* operator->() const {
        return p_.GetFirst();
    }
    T& operator[](size_t i) const {
        return p_.GetFirst()[i];
    }
  
};
