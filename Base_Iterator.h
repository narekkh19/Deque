#ifndef BASE_ITERATOR_H
#define BASE_ITERATOR_H

#include <iterator>
#include <cstddef>
#include "Deque.h"
template <typename T>
class Deque;

template <typename T, bool IsConst = false, bool IsReverse = false>
class BaseIterator {
    public:
        
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using const_reference = const T&;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using Chunkpointer = std::conditional_t<IsConst, const T**, T**>;
    
    private:    
    friend Deque<T>;
    static constexpr size_t CHUNK_SIZE = 128;
    pointer curr;
    pointer first;
    pointer last;
    Chunkpointer node;

    void increment();
    void decrement();

public:
    BaseIterator();
    BaseIterator(pointer _curr, pointer _first, pointer _last, Chunkpointer _node);
    BaseIterator(pointer T_ptr);
    BaseIterator(const BaseIterator& other) noexcept;
    BaseIterator(BaseIterator&& other) noexcept;

    BaseIterator& operator=(const BaseIterator& other) noexcept;
    BaseIterator& operator=(BaseIterator&& other) noexcept;

    BaseIterator operator+(difference_type n) const;
    BaseIterator operator-(difference_type n) const;
    difference_type operator-(const BaseIterator& other) const;

    BaseIterator& operator++();
    BaseIterator operator++(int);
    BaseIterator& operator--();
    BaseIterator operator--(int);

    BaseIterator& operator+=(difference_type n);
    BaseIterator& operator-=(difference_type n);

    pointer operator->() const;
    reference operator*();
    const_reference operator*() const;
    reference operator[](difference_type n);
    const_reference operator[](difference_type n) const;

    bool operator>(const BaseIterator& other) const;
    bool operator<(const BaseIterator& other) const;
    bool operator>=(const BaseIterator& other) const;
    bool operator<=(const BaseIterator& other) const;
    bool operator==(const BaseIterator& other) const;
    bool operator!=(const BaseIterator& other) const;
};

template <typename T>
using Iterator = BaseIterator<T, false, false>;

template <typename T>
using ConstIterator = BaseIterator<T, true, false>;

template <typename T>
using ReverseIterator = BaseIterator<T, false, true>;

template <typename T>
using ConstReverseIterator = BaseIterator<T, true, true>;

#include "Base_Iterator.cpp"
#endif //BASE_ITERATOR_H