#include <memory>

template <typename T>
class List {
private:
    struct Node {
        Node() { }
        Node(T* a)
            : val(a)
        {
        }
        std::unique_ptr<T> val;
        std::shared_ptr<Node> next;
        std::weak_ptr<Node> prev;
    };
    std::shared_ptr<Node> head, tail;
    size_t sz = 0;

public:
    class iterator {
        std::shared_ptr<Node> ptr;

    public:
        iterator(std::shared_ptr<Node> ptr_)
        {
            ptr = ptr_;
        }
        iterator& operator++()
        {
            ptr = ptr->next;
            return *this;
        }
        iterator& operator--()
        {
            ptr = ptr->prev.lock();
            return *this;
        }
        const T& operator*() const
        {
            return *(ptr->val);
        }
        T& operator*()
        {
            return *(ptr->val);
        }
        bool operator==(const iterator& it) const
        {
            return (ptr == it.ptr);
        }
        bool operator!=(const iterator& it) const
        {
            return (ptr != it.ptr);
        }
    };
    List()
    {
        sz = 0;
        head.reset(new Node(nullptr));
        tail.reset(new Node(nullptr));
        head->next = tail;
        tail->prev = head;
    }
    void push_front(const T& a)
    {
        sz++;
        std::shared_ptr<Node> ptr_ = head->next;
        head->next.reset(new Node(new T(a)));
        ptr_->prev = head->next;
        head->next->prev = head;
        head->next->next = ptr_;
    }
    void push_back(const T& a)
    {
        sz++;
        std::shared_ptr<Node> ptr_ = tail->prev.lock();
        tail->prev.lock()->next.reset(new Node(new T(a)));
        tail->prev = tail->prev.lock()->next;
        tail->prev.lock()->next = tail;
        tail->prev.lock()->prev = ptr_;
    }
    void pop_front()
    {
        sz--;
        head->next = head->next->next;
        head->next->prev = head;
    }
    void pop_back()
    {
        sz--;
        tail->prev = tail->prev.lock()->prev;
        tail->prev.lock()->next = tail;
    }
    List& operator=(const List& other)
    {
        if (sz == other.sz && head == other.head && tail == other.tail) {
            return *this;
        }
        while (sz > 0) {
            pop_back();
        }
        for (const auto& val : other) {
            push_back(val);
        }
        return *this;
    }
    List(const List& other)
    {
        sz = 0;
        head.reset(new Node(nullptr));
        tail.reset(new Node(new T(-1)));
        head->next = tail;
        tail->prev = head;
        for (const auto& val : other) {
            push_back(val);
        }
    }
    size_t size() const
    {
        return sz;
    }

    iterator begin() const
    {
        return head->next;
    }
    iterator end() const
    {
        return tail;
    }
    ~List() { }
};