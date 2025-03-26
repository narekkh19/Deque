#ifndef DEQUE_H
#define DEQUE_H
#include <iostream>
#include <vector>

template <typename T>
class Deque {
    private:
        const static size_t CHUNK_SIZE = 128;
        std::vector<T*> chunk_map;

    public:
        Deque () {};
        Deque (size_t _n);
};

#include "Deque.cpp"
#endif