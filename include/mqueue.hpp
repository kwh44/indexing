//
// Created by kwh44 on 5/20/19.
//

#ifndef PARALLEL_INDEXING_MQUEUE_H
#define PARALLEL_INDEXING_MQUEUE_H

#include <queue>
#include <mutex>

#include <condition_variable>


#include <iostream>


template<typename T>
class Mqueue {
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::condition_variable cond_read;
    bool read_status = true;
public:
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.empty()) {
            cond_.wait(lock);
        }
        auto item(std::move(queue_.front()));
        queue_.pop();
        if (queue_.size() < 10 && !read_status) {
            cond_read.notify_one();
        }
        return item;
    }

    void push(T &item) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.emplace(std::move(item));
        lock.unlock();
        cond_.notify_one();
    }

    auto size() {
        std::lock_guard<std::mutex> lk(mutex_);
        return queue_.size();
    }

    void continue_read() {
        read_status = false;
        std::unique_lock<std::mutex> lock(mutex_);
        cond_read.wait(lock);
        read_status = true;
        lock.unlock();
    }
};

#endif //PARALLEL_INDEXING_MQUEUE_H