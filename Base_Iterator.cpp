#include "Base_Iterator.h"

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse>::BaseIterator() = default;

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse>::BaseIterator(
    pointer _curr, pointer _first, pointer _last, Chunkpointer _node)
    : curr{_curr}, first{_first}, last{_last}, node{_node} {}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse>::BaseIterator(pointer T_ptr)
    : curr{T_ptr}, first{nullptr}, last{nullptr}, node{nullptr} {}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse>::BaseIterator(const BaseIterator& other) noexcept 
    : curr{other.curr}, first{other.first}, last{other.last}, node{other.node} {}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse>::BaseIterator(BaseIterator&& other) noexcept 
    : curr{other.curr}, first{other.first}, last{other.last}, node{other.node} {
    other.curr = other.first = other.last = nullptr;
    other.node = nullptr;
}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse>& 
BaseIterator<T, IsConst, IsReverse>::operator=(const BaseIterator& other) noexcept {
    if (this != &other) {
        curr = other.curr;
        first = other.first;
        last = other.last;
        node = other.node;
    }
    return *this;
}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse>& 
BaseIterator<T, IsConst, IsReverse>::operator=(BaseIterator&& other) noexcept {
    if (this != &other) {
        curr = other.curr;
        first = other.first;
        last = other.last;
        node = other.node;
        other.curr = other.first = other.last = nullptr;
        other.node = nullptr;
    }
    return *this;
}
template <typename T, bool IsConst, bool IsReverse>
void BaseIterator<T, IsConst, IsReverse>::increment() {
    if constexpr (!IsReverse) {
        ++curr;
        if (curr > last) {
            ++node;
            if (node == nullptr || *node == nullptr) {
                curr = nullptr;
            } else {
                first = *node;
                last = first + CHUNK_SIZE - 1;
                curr = first;
            }
        }
    } else {
        --curr;
        if (curr < first) {
            --node;
            if (node == nullptr || *node == nullptr) {
                curr = nullptr;
            } else {
                first = *node;
                last = first + CHUNK_SIZE - 1;
                curr = last;
            }
        }
    }
}

template <typename T, bool IsConst, bool IsReverse>
void BaseIterator<T, IsConst, IsReverse>::decrement() {
    if constexpr (!IsReverse) {
        --curr;
        if (curr < first) {
            --node;
            if (node == nullptr || *node == nullptr) {
                curr = nullptr;
            } else {
                first = *node;
                last = first + CHUNK_SIZE - 1;
                curr = last;
            }
        }
    } else {
        ++curr;
        if (curr > last) {
            ++node;
            if (node == nullptr || *node == nullptr) {
                curr = nullptr;
            } else {
                first = *node;
                last = first + CHUNK_SIZE - 1;
                curr = first;
            }
        }
    }
}


template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse> 
BaseIterator<T, IsConst, IsReverse>::operator+(difference_type n) const {
    BaseIterator temp = *this;
    if constexpr (IsReverse) {
        temp -= n;
    } else {
        temp += n;
    }
    return temp;
}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse> 
BaseIterator<T, IsConst, IsReverse>::operator-(difference_type n) const {
    BaseIterator temp = *this;
    if constexpr (IsReverse) {
        temp += n;
    } else {
        temp -= n;
    }
    return temp;
}

template <typename T, bool IsConst, bool IsReverse>
typename BaseIterator<T, IsConst, IsReverse>::difference_type 
BaseIterator<T, IsConst, IsReverse>::operator-(const BaseIterator& other) const {
    if (node == other.node) {
        return curr - other.curr;
    }
    
    BaseIterator start = (node < other.node) ? *this : other;
    BaseIterator end = (node < other.node) ? other : *this;
    
    difference_type diff = 
        (start.last - start.curr + 1) +
        (end.curr - end.first) +
        ((end.node - start.node - 1) * CHUNK_SIZE);
    
    return (node < other.node) ? 
        (IsReverse ? -diff : diff) : 
        (IsReverse ? diff : -diff);
}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse>& 
BaseIterator<T, IsConst, IsReverse>::operator++() {
    increment();
    return *this;
}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse> 
BaseIterator<T, IsConst, IsReverse>::operator++(int) {
    BaseIterator tmp = *this;
    increment();
    return tmp;
}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse>& 
BaseIterator<T, IsConst, IsReverse>::operator--() {
    decrement();
    return *this;
}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse> 
BaseIterator<T, IsConst, IsReverse>::operator--(int) {
    BaseIterator tmp = *this;
    decrement();
    return tmp;
}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse>& 
BaseIterator<T, IsConst, IsReverse>::operator+=(difference_type n) {
    if constexpr (IsReverse) {
        if (n >= 0) {
            difference_type block_size = last - first + 1;
            difference_type blocks_to_jump = n / block_size;
            difference_type offset_in_block = n % block_size;

            node -= blocks_to_jump;
            first = *node;
            last = first + CHUNK_SIZE - 1;
            curr = last - offset_in_block;
        } else {
            return operator-= (-n);
        }
    } else {
        if (n >= 0) {
            difference_type block_size = last - first + 1;
            difference_type blocks_to_jump = n / block_size;
            difference_type offset_in_block = n % block_size;

            node += blocks_to_jump;
            first = *node;
            last = first + CHUNK_SIZE - 1;
            curr = first + offset_in_block;
        } else {
            return operator-= (-n);
        }
    }
    return *this;
}

template <typename T, bool IsConst, bool IsReverse>
BaseIterator<T, IsConst, IsReverse>& 
BaseIterator<T, IsConst, IsReverse>::operator-=(difference_type n) {
    if constexpr (IsReverse) {
        return operator+=(-n);
    } else {
        if (n >= 0) {
            difference_type block_size = last - first + 1;
            difference_type blocks_to_jump = n / block_size;
            difference_type offset_in_block = n % block_size;

            node -= blocks_to_jump;
            first = *node;
            last = first + CHUNK_SIZE - 1;
            curr = last - offset_in_block;
        } else {
            return operator+=(-n);
        }
    }
    return *this;
}

template <typename T, bool IsConst, bool IsReverse>
typename BaseIterator<T, IsConst, IsReverse>::pointer 
BaseIterator<T, IsConst, IsReverse>::operator->() const {
    return curr;
}

template <typename T, bool IsConst, bool IsReverse>
typename BaseIterator<T, IsConst, IsReverse>::reference 
BaseIterator<T, IsConst, IsReverse>::operator*() {
    return *curr;
}

template <typename T, bool IsConst, bool IsReverse>
typename BaseIterator<T, IsConst, IsReverse>::const_reference 
BaseIterator<T, IsConst, IsReverse>::operator*() const {
    return *curr;
}

template <typename T, bool IsConst, bool IsReverse>
typename BaseIterator<T, IsConst, IsReverse>::reference 
BaseIterator<T, IsConst, IsReverse>::operator[](difference_type n) {
    return *(*this + n);
}

template <typename T, bool IsConst, bool IsReverse>
typename BaseIterator<T, IsConst, IsReverse>::const_reference 
BaseIterator<T, IsConst, IsReverse>::operator[](difference_type n) const {
    return *(*this + n);
}

template <typename T, bool IsConst, bool IsReverse>
bool BaseIterator<T, IsConst, IsReverse>::operator>(const BaseIterator& other) const {
    return *this - other > 0;
}

template <typename T, bool IsConst, bool IsReverse>
bool BaseIterator<T, IsConst, IsReverse>::operator<(const BaseIterator& other) const {
    return *this - other < 0;
}

template <typename T, bool IsConst, bool IsReverse>
bool BaseIterator<T, IsConst, IsReverse>::operator>=(const BaseIterator& other) const {
    return *this - other >= 0;
}

template <typename T, bool IsConst, bool IsReverse>
bool BaseIterator<T, IsConst, IsReverse>::operator<=(const BaseIterator& other) const {
    return *this - other <= 0;
}

template <typename T, bool IsConst, bool IsReverse>
bool BaseIterator<T, IsConst, IsReverse>::operator==(const BaseIterator& other) const {
    return *this - other == 0;
}

template <typename T, bool IsConst, bool IsReverse>
bool BaseIterator<T, IsConst, IsReverse>::operator!=(const BaseIterator& other) const {
    return *this - other != 0;
}
