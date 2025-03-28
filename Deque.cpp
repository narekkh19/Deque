#include "Deque.h"

template <typename T>
Deque<T>::Deque() : chunk_map{}, start{}, finish{} {}  


// Constructor: create a deque with n elements
template <typename T>
Deque<T>::Deque(size_t n) {
    // If n is zero, initialize an empty deque.
    if (n == 0) {
        num_elements = 0;
        start = BaseIterator<T, false, false>(); // default iterator
        finish = BaseIterator<T, false, false>(); 
        return;
    }
    // Calculate number of chunks needed
    size_t num_chunks = n / CHUNK_SIZE;
    if (n % CHUNK_SIZE != 0) {
        ++num_chunks;
    }
    num_elements = n;
    chunk_map.resize(num_chunks);
    
    // Allocate each chunk and initialize with default values.
    for (size_t i = 0; i < num_chunks; ++i) {
        chunk_map[i] = std::make_unique<T[]>(CHUNK_SIZE);
        for (size_t j = 0; j < CHUNK_SIZE; ++j) {
            chunk_map[i][j] = T{};
        }
    }
    
    // Initialize start iterator with the first chunk
    start = BaseIterator<T, false, false>(
        chunk_map[0].get(),          // current pointer in first chunk
        chunk_map[0].get(),          // pointer to beginning of first chunk
        chunk_map[0].get() + CHUNK_SIZE - 1, // pointer to end of first chunk
        chunk_map.data()             // pointer to the chunk map array
    );
    // Compute offset for the finish iterator; if n is a multiple of CHUNK_SIZE use CHUNK_SIZE.
    size_t offset = (n % CHUNK_SIZE == 0 && n != 0) ? CHUNK_SIZE : (n % CHUNK_SIZE);
    size_t last_chunk = chunk_map.size() - 1;
    finish = BaseIterator<T, false, false>(
        chunk_map[last_chunk].get() + offset - 1, // finish points to last valid element
        chunk_map[last_chunk].get(),              // beginning of last chunk
        chunk_map[last_chunk].get() + CHUNK_SIZE - 1, // end of last chunk
        chunk_map.data()                          // pointer to chunk map array
    );
}

// Copy constructor: deep copy from another deque
template <typename T>
Deque<T>::Deque(const Deque<T>& other) : num_elements(other.num_elements), chunk_map(other.chunk_map.size()) {
    num_elements = other.num_elements;
    for (size_t i = 0; i < other.chunk_map.size(); ++i) {
        chunk_map[i] = std::make_unique<T[]>(CHUNK_SIZE);
        for (size_t j = 0; j < CHUNK_SIZE; ++j) {
            chunk_map[i][j] = other.chunk_map[i][j];
        }
    }
    // Calculate iterator offsets for start and finish
    size_t start_index = other.start.curr - other.start.first;
    size_t finish_index = other.finish.curr - other.finish.first;
    start = BaseIterator<T, false, false>(
        chunk_map[0].get() + start_index,
        chunk_map[0].get(),
        chunk_map[0].get() + CHUNK_SIZE - 1,
        chunk_map.data()
    );
    size_t last_chunk = chunk_map.size() - 1;
    finish = BaseIterator<T, false, false>(
        chunk_map[last_chunk].get() + finish_index,
        chunk_map[last_chunk].get(),
        chunk_map[last_chunk].get() + CHUNK_SIZE - 1,
        chunk_map.data()
    );
}

// Move constructor: transfer ownership of resources
template <typename T>
Deque<T>::Deque(Deque<T>&& other) noexcept 
    : num_elements(other.num_elements),
      chunk_map(std::move(other.chunk_map)),
      start(other.start),
      finish(other.finish) {
    other.num_elements = 0;
}

// Add an element to the back of the deque
template <typename T>
void Deque<T>::push_back(const T& val) {
    ++num_elements;
    // If finish.curr is not set, allocate a new chunk
    if (!finish.curr) {
        chunk_map.push_back(std::make_unique<T[]>(CHUNK_SIZE));
        start.first = finish.first = chunk_map.back().get();
        start.last = finish.last = start.first + CHUNK_SIZE - 1;
        start.curr = finish.curr = start.first;
        *finish.curr = val;
        return;
    }
    // If there is room in the current chunk, use it
    if (finish.curr < finish.last) {
        ++finish.curr;
        *finish.curr = val;
        return;
    }
    // Otherwise, allocate a new chunk and update finish iterator
    chunk_map.push_back(std::make_unique<T[]>(CHUNK_SIZE));
    finish.first = chunk_map.back().get();
    finish.last = finish.first + CHUNK_SIZE - 1;
    finish.curr = finish.first;
    *finish.curr = val;
}

// Add an element to the front of the deque
template <typename T>
void Deque<T>::push_front(const T& val) {
    ++num_elements;
    if (!start.curr) {
        // If deque is empty, create a new chunk
        chunk_map.push_back(std::make_unique<T[]>(CHUNK_SIZE));
        start.first = finish.first = chunk_map.back().get();
        start.last = finish.last = start.first + CHUNK_SIZE - 1;
        start.curr = finish.curr = start.first;
        *start.curr = val;
        return;
    }
    // If there is room before the current start, move back the start pointer
    if (start.curr > start.first) {
        --start.curr;
        *start.curr = val;
        return;
    }
    // Otherwise, insert a new chunk at the beginning
    chunk_map.insert(chunk_map.begin(), std::make_unique<T[]>(CHUNK_SIZE));
    start.first = chunk_map.front().get();
    start.last = start.first + CHUNK_SIZE - 1;
    start.curr = start.last;
    *start.curr = val;
    // If finish.first is null, update finish too
    if (finish.first == nullptr) {
        finish = start;
    }
}

// Remove an element from the back of the deque
template <typename T>
void Deque<T>::pop_back() {
    if (!finish.curr) {
        throw std::out_of_range("Cannot pop from an empty deque");
    }
    --num_elements;
    // If not at the first element of the chunk, simply move the pointer back
    if (finish.curr > finish.first) {
        --finish.curr;
        return;
    }
    // Otherwise, remove the last chunk
    chunk_map.pop_back();
    if (chunk_map.empty()) {
        start.curr = finish.curr = nullptr;
        start.first = start.last = nullptr;
        finish.first = finish.last = nullptr;
        return;
    }
    finish.first = chunk_map.back().get();
    finish.last = finish.first + CHUNK_SIZE - 1;
    finish.curr = finish.last;
}

// Remove an element from the front of the deque
template <typename T>
void Deque<T>::pop_front() {
    if (!start.curr) {
        throw std::out_of_range("Cannot pop from an empty deque");
    }
    --num_elements;
    // If there are more elements in the first chunk, move the pointer forward
    if (start.curr < start.last) {
        ++start.curr;
        return;
    }
    // Otherwise, remove the first chunk
    chunk_map.erase(chunk_map.begin());
    if (chunk_map.empty()) {
        start.curr = finish.curr = nullptr;
        start.first = start.last = nullptr;
        finish.first = finish.last = nullptr;
        return;
    }
    start.first = chunk_map.front().get();
    start.last = start.first + CHUNK_SIZE - 1;
    start.curr = start.first;
}

// Insert an element at a given iterator position
template <typename T>
BaseIterator<T, false, false> Deque<T>::insert(BaseIterator<T, false, false> pos, const T& val) {
    if (chunk_map.empty()) {
        // If empty, push_front
        chunk_map.push_back(std::make_unique<T[]>(CHUNK_SIZE));
        start.first = finish.first = chunk_map.back().get();
        start.last = finish.last = start.first + CHUNK_SIZE - 1;
        start.curr = finish.curr = start.first;
        *start.curr = val;
        return start;
    }
    if (pos == start) {
        push_front(val);
        return start;
    }
    if (pos == finish) {
        push_back(val);
        return finish;
    }
    if (pos.curr < pos.last && pos.curr > pos.first) {
        // Shift elements in the current chunk to make room
        T* insert_pos = pos.curr;
        T* current = finish.curr;
        while (current >= insert_pos) {
            *(current + 1) = *current;
            --current;
        }
        *insert_pos = val;
        ++finish.curr;
        return BaseIterator<T, false, false>(pos.first, insert_pos, pos.last, chunk_map.data(), 0);
    }
    // Otherwise, allocate a new chunk and insert there
    T* new_chunk = new T[CHUNK_SIZE];
    size_t chunk_index = std::find(chunk_map.begin(), chunk_map.end(), pos.first) - chunk_map.begin();
    chunk_map.insert(chunk_map.begin() + chunk_index + 1, std::make_unique<T[]>(CHUNK_SIZE));
    T* current = pos.curr;
    T* new_chunk_ptr = new_chunk;
    while (current <= pos.last) {
        *new_chunk_ptr = *current;
        ++new_chunk_ptr;
        ++current;
    }
    *new_chunk_ptr = val;
    chunk_map[chunk_index + 1] = std::unique_ptr<T[]>(new_chunk);
    return BaseIterator<T, false, false>(new_chunk, new_chunk + (pos.curr - pos.first), new_chunk + CHUNK_SIZE - 1, chunk_map.data(), chunk_index + 1);
}

// Construct an element in-place at a given position
template <typename T>
template <typename... Args>
BaseIterator<T, false, false> Deque<T>::emplace(BaseIterator<T, false, false> pos, Args&&... args) {
    if (chunk_map.empty()) {
        chunk_map.push_back(std::make_unique<T[]>(CHUNK_SIZE));
        start.first = finish.first = chunk_map.back().get();
        start.last = finish.last = start.first + CHUNK_SIZE - 1;
        start.curr = finish.curr = start.first;
        new (start.curr) T(std::forward<Args>(args)...);
        return start;
    }
    if (pos == start) {
        chunk_map.front().get()[0] = T(std::forward<Args>(args)...);
        return start;
    }
    if (pos == finish) {
        ++finish.curr;
        new (finish.curr) T(std::forward<Args>(args)...);
        return finish;
    }
    if (pos.curr < pos.last && pos.curr > pos.first) {
        T* insert_pos = pos.curr;
        T* current = finish.curr;
        while (current >= insert_pos) {
            *(current + 1) = std::move(*current);
            --current;
        }
        new (insert_pos) T(std::forward<Args>(args)...);
        ++finish.curr;
        return BaseIterator<T, false, false>(pos.first, insert_pos, pos.last, chunk_map.data(), 0);
    }
    T* new_chunk = new T[CHUNK_SIZE];
    size_t chunk_index = std::find(chunk_map.begin(), chunk_map.end(), pos.first) - chunk_map.begin();
    chunk_map.insert(chunk_map.begin() + chunk_index + 1, std::make_unique<T[]>(CHUNK_SIZE));
    T* current = pos.curr;
    T* new_chunk_ptr = new_chunk;
    while (current <= pos.last) {
        new (new_chunk_ptr) T(std::move(*current));
        ++new_chunk_ptr;
        ++current;
    }
    new (new_chunk_ptr) T(std::forward<Args>(args)...);
    chunk_map[chunk_index + 1] = std::unique_ptr<T[]>(new_chunk);
    return BaseIterator<T, false, false>(new_chunk, new_chunk + (pos.curr - pos.first), new_chunk + CHUNK_SIZE - 1, chunk_map.data(), chunk_index + 1);
}

// Clear all elements from the deque
template <typename T>
void Deque<T>::clear() {
    for (auto& chunk : chunk_map) {
        T* start_ptr = chunk.get();
        T* end_ptr = start_ptr + CHUNK_SIZE;
        for (T* ptr = start_ptr; ptr < end_ptr; ++ptr) {
            ptr->~T();
        }
    }
    chunk_map.clear();
    start.curr = finish.curr = nullptr;
    start.first = start.last = nullptr;
    finish.first = finish.last = nullptr;
}

// Erase an element at a given position and return an iterator to the next element
template <typename T>
BaseIterator<T, false, false> Deque<T>::erase(BaseIterator<T, false, false> pos) {
    if (chunk_map.empty()) {
        throw std::out_of_range("Cannot erase from an empty deque");
    }
    if (pos == start) {
        pop_front();
        return start;
    }
    if (pos == finish) {
        pop_back();
        return finish;
    }
    T* erase_pos = pos.curr;
    T* current = erase_pos;
    erase_pos->~T();
    while (current < finish.curr) {
        *current = std::move(*(current + 1));
        ++current;
    }
    --finish.curr;
    if (start.curr > start.last) {
        chunk_map.erase(std::find(chunk_map.begin(), chunk_map.end(), pos.first));
        if (chunk_map.empty()) {
            start.curr = finish.curr = nullptr;
            start.first = start.last = nullptr;
            finish.first = finish.last = nullptr;
        } else {
            start.first = chunk_map.front().get();
            start.last = start.first + CHUNK_SIZE - 1;
            start.curr = start.first;
        }
    }
    return BaseIterator<T, false, false>(pos.first, erase_pos, pos.last, chunk_map.data(), 0);
}

// Emplace an element at the back of the deque
template <typename T>
template <typename... Args>
void Deque<T>::emplace_back(Args&&... args) {
    if (!finish.curr || finish.curr >= finish.last) {
        chunk_map.push_back(std::make_unique<T[]>(CHUNK_SIZE));
        finish.first = chunk_map.back().get();
        finish.last = finish.first + CHUNK_SIZE - 1;
        finish.curr = finish.first;
        if (chunk_map.size() == 1) {
            start = finish;
        }
    }
    new (finish.curr) T(std::forward<Args>(args)...);
    ++finish.curr;
}

// Resize the deque to new_size, initializing new elements with val if expanding
template <typename T>
void Deque<T>::resize(size_t new_size, const T& val) {
    size_t current_size = size();
    if (new_size > current_size) {
        size_t elements_to_add = new_size - current_size;
        while (elements_to_add > 0) {
            size_t space_in_current_chunk = (finish.last - finish.curr) + 1;
            size_t elements_to_add_in_chunk = std::min(space_in_current_chunk, elements_to_add);
            for (size_t i = 0; i < elements_to_add_in_chunk; ++i) {
                emplace_back(val);
            }
            elements_to_add -= elements_to_add_in_chunk;
            if (elements_to_add > 0) {
                chunk_map.push_back(std::make_unique<T[]>(CHUNK_SIZE));
                finish.first = chunk_map.back().get();
                finish.last = finish.first + CHUNK_SIZE - 1;
                finish.curr = finish.first;
            }
        }
    } else if (new_size < current_size) {
        size_t elements_to_remove = current_size - new_size;
        while (elements_to_remove > 0) {
            if (finish.curr == finish.first) {
                chunk_map.pop_back();
                if (!chunk_map.empty()) {
                    finish.first = chunk_map.back().get();
                    finish.last = finish.first + CHUNK_SIZE - 1;
                    finish.curr = finish.last;
                } else {
                    finish = start = {};
                    break;
                }
            }
            size_t elements_to_remove_in_chunk = std::min(static_cast<size_t>(finish.curr - finish.first + 1), elements_to_remove);
            finish.curr -= elements_to_remove_in_chunk;
            elements_to_remove -= elements_to_remove_in_chunk;
        }
    }
}

// Swap this deque with another deque
template <typename T>
void Deque<T>::swap(Deque<T>& other) noexcept {
    std::swap(chunk_map, other.chunk_map);
    std::swap(start.first, other.start.first);
    std::swap(start.curr, other.start.curr);
    std::swap(start.last, other.start.last);
    std::swap(finish.first, other.finish.first);
    std::swap(finish.curr, other.finish.curr);
    std::swap(finish.last, other.finish.last);
}

// Copy assignment operator
template <typename T>
Deque<T>& Deque<T>::operator=(const Deque& other) {
    if (this != &other) {
        clear();
        for (const auto& chunk : other.chunk_map) {
            chunk_map.push_back(std::make_unique<T[]>(CHUNK_SIZE));
            std::copy(chunk.get(), chunk.get() + CHUNK_SIZE, chunk_map.back().get());
        }
        if (!chunk_map.empty()) {
            size_t start_offset = other.start.curr - other.start.first;
            size_t finish_offset = other.finish.curr - other.finish.first;
            start.first = chunk_map[0].get();
            start.last = start.first + CHUNK_SIZE - 1;
            start.curr = start.first + start_offset;
            size_t finish_chunk_index = other.chunk_map.size() - 1;
            finish.first = chunk_map[finish_chunk_index].get();
            finish.last = finish.first + CHUNK_SIZE - 1;
            finish.curr = finish.first + finish_offset;
        }
    }
    return *this;
}

// Move assignment operator
template <typename T>
Deque<T>& Deque<T>::operator=(Deque&& other) noexcept {
    if (this != &other) {
        clear();
        chunk_map = std::move(other.chunk_map);
        start = other.start;
        finish = other.finish;
        other.start.curr = other.start.first = other.start.last = nullptr;
        other.finish.curr = other.finish.first = other.finish.last = nullptr;
    }
    return *this;
}

// Access element at pos with bounds checking
template <typename T>
T& Deque<T>::at(size_t pos) {
    size_t total_size = size();
    if (pos >= total_size) {
        throw std::out_of_range("Deque index out of range");
    }
    size_t cumulative_size = 0;
    for (const auto& chunk : chunk_map) {
        size_t current_chunk_size = (chunk == chunk_map.back()) ? (finish.curr - chunk.get() + 1) : CHUNK_SIZE;
        if (cumulative_size + current_chunk_size > pos) {
            return chunk.get()[pos - cumulative_size];
        }
        cumulative_size += current_chunk_size;
    }
    throw std::out_of_range("Unexpected error in at method");
}

template <typename T>
const T& Deque<T>::at(size_t pos) const {
    size_t total_size = size();
    if (pos >= total_size) {
        throw std::out_of_range("Deque index out of range");
    }
    size_t cumulative_size = 0;
    for (const auto& chunk : chunk_map) {
        size_t current_chunk_size = (chunk == chunk_map.back()) ? (finish.curr - chunk.get() + 1) : CHUNK_SIZE;
        if (cumulative_size + current_chunk_size > pos) {
            return chunk.get()[pos - cumulative_size];
        }
        cumulative_size += current_chunk_size;
    }
    throw std::out_of_range("Unexpected error in at method");
}

// Return first element
template <typename T>
T& Deque<T>::front() {
    if (num_elements == 0) {
        throw std::out_of_range("Deque is empty");
    }
    return *start.curr;
}

template <typename T>
const T& Deque<T>::front() const {
    if (num_elements == 0) {
        throw std::out_of_range("Deque is empty");
    }
    return *start.curr;
}

// Return last element
template <typename T>
T& Deque<T>::back() {
    if (num_elements == 0) {
        throw std::out_of_range("Deque is empty");
    }
    return *finish.curr;
}

template <typename T>
const T& Deque<T>::back() const {
    if (num_elements == 0) {
        throw std::out_of_range("Deque is empty");
    }
    return *finish.curr;
}

// Operator[] uses at() for bounds checking
template <typename T>
T& Deque<T>::operator[](size_t pos) {
    return at(pos);
}

template <typename T>
const T& Deque<T>::operator[](size_t pos) const {
    return at(pos);
}

// Comparison operators
template <typename T>
bool operator==(const Deque<T>& lhs, const Deque<T>& rhs) {
    if (lhs.size() != rhs.size()) return false;
    auto lhs_it = lhs.begin();
    auto rhs_it = rhs.begin();
    while (lhs_it != lhs.end()) {
        if (*lhs_it != *rhs_it) return false;
        ++lhs_it;
        ++rhs_it;
    }
    return true;
}

template <typename T>
bool operator!=(const Deque<T>& lhs, const Deque<T>& rhs) {
    return !(lhs == rhs);
}

template <typename T>
bool operator<(const Deque<T>& lhs, const Deque<T>& rhs) {
    auto lhs_it = lhs.begin();
    auto rhs_it = rhs.begin();
    while (lhs_it != lhs.end() && rhs_it != rhs.end()) {
        if (*lhs_it < *rhs_it) return true;
        if (*rhs_it < *lhs_it) return false;
        ++lhs_it;
        ++rhs_it;
    }
    return lhs.size() < rhs.size();
}

template <typename T>
bool operator>(const Deque<T>& lhs, const Deque<T>& rhs) {
    return rhs < lhs;
}

template <typename T>
bool operator<=(const Deque<T>& lhs, const Deque<T>& rhs) {
    return !(rhs < lhs);
}

template <typename T>
bool operator>=(const Deque<T>& lhs, const Deque<T>& rhs) {
    return !(lhs < rhs);
}

// Begin returns iterator to first element
template <typename T>
BaseIterator<T, false, false> Deque<T>::begin() {
    if (num_elements == 0) {
        return BaseIterator<T, false, false>();
    }
    return start;
}

// End returns iterator one past the last element
template <typename T>
BaseIterator<T, false, false> Deque<T>::end() {
    if (num_elements == 0) {
        return BaseIterator<T, false, false>();
    }
    BaseIterator<T, false, false> end_iter = finish;
    if (end_iter.curr != nullptr) {
        ++end_iter.curr;
    }
    return end_iter;
}

template <typename T>
BaseIterator<T, false, true> Deque<T>::cend() const {
    BaseIterator<T, false, true> end_iter(finish.first, finish.curr, finish.last);
    if (end_iter.curr != nullptr) {
        ++end_iter.curr;
    }
    return end_iter;
}

template <typename T>
BaseIterator<T, true, false> Deque<T>::rbegin() {
    return BaseIterator<T, true, false>(finish.first, finish.curr, finish.last);
}

template <typename T>
BaseIterator<T, true, true> Deque<T>::crbegin() const {
    return BaseIterator<T, true, true>(finish.first, finish.curr, finish.last);
}

template <typename T>
BaseIterator<T, true, false> Deque<T>::rend() {
    BaseIterator<T, true, false> rend_iter = start;
    if (rend_iter.curr != nullptr) {
        --rend_iter.curr;
    }
    return rend_iter;
}

template <typename T>
BaseIterator<T, true, true> Deque<T>::crend() const {
    BaseIterator<T, true, true> rend_iter(start.first, start.curr, start.last);
    if (rend_iter.curr != nullptr) {
        --rend_iter.curr;
    }
    return rend_iter;
}

// Check if deque is empty
template <typename T>
bool Deque<T>::empty() const {
    return num_elements == 0;
}

// Calculate the size of the deque
template <typename T>
size_t Deque<T>::size() const {
    if (num_elements == 0) return 0;
    size_t total_size = (chunk_map.size() - 1) * CHUNK_SIZE + (finish.curr - finish.first + 1);
    return total_size;
}

template <typename T>
size_t Deque<T>::max_size() const {
    return std::numeric_limits<size_t>::max() / sizeof(T);
}

// Shrink the allocated memory to fit current size
template <typename T>
void Deque<T>::shrink_to_fit() {
    while (chunk_map.size() > 1 && (finish.curr - finish.first + 1) < CHUNK_SIZE) {
        chunk_map.pop_back();
        finish.first = chunk_map.back().get();
        finish.last = finish.first + CHUNK_SIZE - 1;
        finish.curr = finish.last;
    }
}
