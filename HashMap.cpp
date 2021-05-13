#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <utility>
#include <vector>
#include<exception>
#include<stdexcept>

template <typename KeyType, typename ValueType, typename Hash = std::hash<KeyType>>
class HashMap {
public:
    typedef typename std::list<std::pair<const KeyType, ValueType>>::iterator iterator;
    typedef typename std::list<std::pair<const KeyType, ValueType>>::const_iterator const_iterator;

private:
    std::vector<std::vector<iterator>> table;
    std::list<std::pair<const KeyType, ValueType>> element;
    Hash hasher;
    size_t sz_element = 0, sz_table = 1;
    bool if_elem_exist(const std::pair<const KeyType &, size_t> & a) const
    {
        for (auto i : table[(a.second % sz_table)]) {
            if (i->first == a.first) {
                return true;
            }
        }
        return false;
    }
    void rebuild()
    {
        table.clear();
        table.resize(sz_table);
        for (auto i = element.begin(); i != element.end(); i++) {
            size_t ha = hasher(i->first);
            table[(ha % sz_table)].push_back(i);
        }
    }
    void ins(const std::pair<KeyType, ValueType>& a)
    {
        if (sz_element == sz_table) {
            sz_table *= 2;
            rebuild();
        }
        size_t ha = hasher(a.first);
        if (!if_elem_exist({ a.first, ha })) {
            element.push_back(a);
            iterator iter_end = element.end();
            table[(ha % sz_table)].push_back(std::prev(iter_end));
            sz_element++;
        }
    }

public:
    HashMap(Hash hasher_ = Hash())
        : hasher(hasher_)
    {
        table.resize(1);
    };
    template <typename Iter>
    HashMap(Iter begin_, Iter end_, Hash hasher_ = Hash())
        : hasher(hasher_)
    {
        table.resize(1);
        while (begin_ != end_) {
            std::pair<KeyType, ValueType> a = *(begin_);
            ins(a);
            begin_++;
        }
    }
    HashMap(const std::initializer_list<std::pair<KeyType, ValueType>>& list_, Hash hasher_ = Hash())
        : hasher(hasher_)
    {
        table.resize(1);
       for (const auto& value : list_) {
           ins(value);
       }
    }
    HashMap(const HashMap& other)
        : hasher(other.hasher)
    {

        table.resize(1);
        for (const auto& value : other) {
            ins(value);
        }
    }
    HashMap& operator=(const HashMap& other)
    {
        if (this == &other) {
            return *this;
        }
        clear();
        for (const auto& value : other) {
            ins(value);
        }
        return *this;
    }

    int size() const
    {
        return sz_element;
    }
    bool empty() const
    {
        return sz_element == 0;
    }
    void insert(const std::pair<KeyType, ValueType>& elem)
    {
        ins(elem);
    }
    void erase(const KeyType& key)
    {
        size_t ha = (hasher(key) % sz_table);
        for (size_t i = 0; i < table[ha].size(); i++) {
            if (table[ha][i]->first == key) {
                element.erase(table[ha][i]);
                table[ha].erase(table[ha].begin() + i);
                sz_element--;
                break;
            }
        }
    }
    iterator begin()
    {
        return element.begin();
    }
    iterator end()
    {
        return element.end();
    }
    const_iterator begin() const
    {
        return element.begin();
    }
    const_iterator end() const
    {
        return element.end();
    }
    Hash hash_function() const
    {
        return hasher;
    }
    iterator find(const KeyType& key)
    {
        size_t ha = hasher(key) % sz_table;
        for (size_t i = 0; i < table[ha].size(); i++) {
            if (table[ha][i]->first == key) {
                return table[ha][i];
            }
        }
        return element.end();
    }
    const_iterator find(const KeyType& key) const
    {
        size_t ha = hasher(key) % sz_table;
        for (size_t i = 0; i < table[ha].size(); i++) {
            if (table[ha][i]->first == key) {
                return table[ha][i];
            }
        }
        return element.end();
    }
    ValueType& operator[](const KeyType& key)
    {
        ins({ key, ValueType() });
        return find(key)->second;
    }
    const ValueType& at(const KeyType& key) const
    {
        if (!if_elem_exist({ key, hasher(key) })) {
            throw std::out_of_range("out_of_range");
        } else {
            return find(key)->second;
        }
    }
    void clear()
    {
        table.clear();
        element.clear();
        sz_element = 0;
        sz_table = 1;
        table.resize(1);
    }
};
