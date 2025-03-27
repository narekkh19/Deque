#ifndef DEQUE_H
#define DEQUE_H
#include <iostream>
#include <vector>
#include "Base_Iterator.h"


template <typename T, bool IsConst, bool IsReverse>
class BaseIterator;

template <typename T>
class Deque {
    private:
        friend class BaseIterator<T, false, false>;
        static constexpr size_t CHUNK_SIZE = 128;
        std::vector<T*> chunk_map;

    public:
        Deque () {};
        Deque (size_t _n);
};

#include "Deque.cpp"
#endif