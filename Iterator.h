#ifndef ITERATOR_H
#define ITERATOR_H
#include "Deque.h"

template <typename T>
class Iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = T&;
        using pointer = T*;
        using Chunkpointer = T**;

    private:    
        pointer curr;
        pointer first;
        pointer last;
        Chunkpointer node;

        Iterator () {};
        Iterator (pointer _curr, pointer _first, pointer _last, Chunkpointer _node) : curr {_curr}, first {_first}, last {_last}, node {_node} {}


        Iterator& operator+ (difference_type n) {
            Iterator tmp = 
        }

    
        
};


#endif //ITERATOR_H