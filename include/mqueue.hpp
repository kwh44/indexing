//
// Created by kwh44 on 5/20/19.
//

#ifndef PARALLEL_INDEXING_MQUEUE_H
#define PARALLEL_INDEXING_MQUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>


template<typename T>
class Mqueue {
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
public:
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.empty()) {
            cond_.wait(lock);
        }
        auto item = queue_.front();
        queue_.pop();
        return item;
    }

    void pop(T &item) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.empty()) {
            cond_.wait(lock);
        }
        item = queue_.front();
        queue_.pop();
    };


    void push(const T &item) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(item);
        lock.unlock();
        cond_.notify_one();
    }


    void push(T &&item) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(std::move(item));
        lock.unlock();
        cond_.notify_one();
    }

    unsigned long size() {
        return queue_.size();
    }


};


//
//class Mqueue
//{
//private:
//    std::queue<T> queue_;
//    std::mutex mutex_;
//    std::condition_variable cond_;
//public:
//    T pop();
//
//    void pop(T &item);
//
//    void push(const T &item);
//
//    void push(T &&item);
//
//    unsigned long size();
//
//
//};

#endif //PARALLEL_INDEXING_MQUEUE_H

