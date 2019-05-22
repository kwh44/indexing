//#include "mqueue.hpp"
//
//
//template<typename T>
//T Mqueue<T>::pop() {
//    std::unique_lock<std::mutex> lock(mutex_);
//    while (queue_.empty()) {
//        cond_.wait(lock);
//    }
//    auto item = queue_.front();
//    queue_.pop();
//    return item;
//}
//
//template<typename T>
//void Mqueue<T>::pop(T &item) {
//    std::unique_lock<std::mutex> lock(mutex_);
//    while (queue_.empty()) {
//        cond_.wait(lock);
//    }
//    item = queue_.front();
//    queue_.pop();
//};
//
//template<typename T>
//void Mqueue<T>::push(const T &item) {
//    std::unique_lock<std::mutex> lock(mutex_);
//    queue_.push(item);
//    lock.unlock();
//    cond_.notify_one();
//}
//
//template<typename T>
//void Mqueue<T>::push(T &&item) {
//    std::unique_lock<std::mutex> lock(mutex_);
//    queue_.push(std::move(item));
//    lock.unlock();
//    cond_.notify_one();
//}
//
//template<typename T>
//unsigned long Mqueue<T>::size() {
//    return queue_.size();
//}

