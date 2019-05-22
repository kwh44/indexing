//
// Created by kwh44 on 5/20/19.
//

#ifndef PARALLEL_INDEXING_MQUEUE_H
#define PARALLEL_INDEXING_MQUEUE_H

#include <deque>

template<class T>
class m_queue {
    std::deque<T> backend_array;
public:
    m_queue() = default;

    // void push(T item) { backend_array.push_back(item); }

    T &front() { return backend_array.front(); }

    void pop() { backend_array.pop_front(); }

    auto emplace_back(T &item) { return backend_array.emplace_back(std::move(item)); }

    auto emplace_back(T &&item) { return backend_array.emplace_back(std::move(item)); }


    size_t size() const { return backend_array.size(); }

    size_t empty() const { return backend_array.empty(); }

    T& operator[](size_t pos) const { return backend_array[pos]; }
};

#endif //PARALLEL_INDEXING_MQUEUE_H

