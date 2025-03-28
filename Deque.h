#ifndef DEQUE_H
#define DEQUE_H

#include <iostream>
#include <vector>
#include <memory>
#include <limits>
#include <algorithm>
#include "Base_Iterator.h"

template <typename T, bool IsConst, bool IsReverse>
class BaseIterator;

template <typename T>
class Deque {
    private:
        friend class BaseIterator<T, false, false>; 
        static constexpr size_t CHUNK_SIZE = 128;   // Fixed chunk size for memory allocation
        
        BaseIterator<T, false, false> start;  
        BaseIterator<T, false, false> finish; 
        std::vector<std::unique_ptr<T[]>> chunk_map; // Stores dynamically allocated chunks
        size_t num_elements; 

    public:
        // Constructors
        Deque();  
        Deque(size_t n); 
        Deque(const Deque<T>& other); 
        Deque(Deque<T>&& other) noexcept; 

        // Assignment operators
        Deque<T>& operator=(const Deque& other); 
        Deque<T>& operator=(Deque&& other) noexcept; 

        // Modifiers
        void push_back(const T& val);  
        void push_front(const T& val); 
        void pop_back();  
        void pop_front(); 
        
        BaseIterator<T, false, false> insert(BaseIterator<T, false, false> pos, const T& val); 
        template <typename... Args>
        BaseIterator<T, false, false> emplace(BaseIterator<T, false, false> pos, Args&&... args); 
        void clear();  
        BaseIterator<T, false, false> erase(BaseIterator<T, false, false> pos);
        template <typename... Args>
        void emplace_back(Args&&... args); 
        void resize(size_t new_size, const T& val = T()); 
        void swap(Deque<T>& other) noexcept; 

        // Element access
        T& at(size_t pos);  
        const T& at(size_t pos) const;
        T& front();  
        const T& front() const;
        T& back();   
        const T& back() const;
        T& operator[](size_t pos); 
        const T& operator[](size_t pos) const;
        
        // Comparison operators
        template <typename U>
        friend bool operator>(const Deque<U>& lhs, const Deque<U>& rhs);
        template <typename U>
        friend bool operator<(const Deque<U>& lhs, const Deque<U>& rhs);
        template <typename U>
        friend bool operator>=(const Deque<U>& lhs, const Deque<U>& rhs);
        template <typename U>
        friend bool operator<=(const Deque<U>& lhs, const Deque<U>& rhs);
        template <typename U>
        friend bool operator==(const Deque<U>& lhs, const Deque<U>& rhs);
        template <typename U>
        friend bool operator!=(const Deque<U>& lhs, const Deque<U>& rhs);

        // Iterators
        BaseIterator<T, false, false> begin();  
        BaseIterator<T, false, true> cbegin() const;
        BaseIterator<T, true, false> rbegin();  
        BaseIterator<T, true, true> crbegin() const;
        BaseIterator<T, false, false> end();    
        BaseIterator<T, false, true> cend() const;
        BaseIterator<T, true, false> rend();    
        BaseIterator<T, true, true> crend() const;
        
        // Capacity
        bool empty() const;  
        size_t size() const; 
        size_t max_size() const; 
        void shrink_to_fit(); 
};

#include "Deque.cpp" 
#endif
