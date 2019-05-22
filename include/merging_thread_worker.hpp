//
// Created by kwh44 on 5/21/19.
//

#ifndef INDEXING_MERGING_THREAD_WORKER_H
#define INDEXING_MERGING_THREAD_WORKER_H

#include "mqueue.hpp"
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>
#include <algorithm>

void merge_worker(m_queue<std::unique_ptr<std::map<std::string, size_t>>> &merge_queue,
                  std::mutex &merge_mtx,
                  std::condition_variable &merge_cv) {
    auto tls_map = std::make_unique<std::map<std::string, size_t> >();
    bool status = true;
    while (status) {
        std::unique_lock<std::mutex> lc(merge_mtx);
        merge_cv.wait(lc, [&merge_queue] { return !merge_queue.empty(); });
        if (merge_queue.front()->empty() && merge_queue.size() >= 1) {
            while (merge_queue.front()->empty()) {
                merge_queue.pop();
            }
            if (merge_queue.size() == 0) {
                // push poisson pill
                merge_queue.emplace_back(std::make_unique<std::map<std::string, size_t>>());
                // push local map
                merge_queue.emplace_back(std::move(tls_map));
                // status = 0
                status = false;
            } else if (merge_queue.size() == 1) {
                // pop from it
                auto new_map = std::move(merge_queue.front());
                // merge with local map
                std::for_each(new_map->begin(), new_map->end(),
                              [&](const std::map<std::string, size_t>::value_type &v) {
                                  tls_map->operator[](v.first) += v.second;
                              });
                // push poisson pill
                merge_queue.emplace_back(std::make_unique<std::map<std::string, size_t>>());
                // push local map
                merge_queue.emplace_back(std::move(tls_map));
                status = false;
                merge_queue.pop();
            } else if (merge_queue.size() > 1) {
                // pop from it
                auto new_map = std::move(merge_queue.front());
                // merge with local map
                std::for_each(new_map->begin(), new_map->end(),
                              [&](const std::map<std::string, size_t>::value_type &v) {
                                  tls_map->operator[](v.first) += v.second;
                              });
                merge_queue.pop();
            } else {

            }
        } else {
            if (merge_queue.size() == 1) {
                merge_queue.emplace_back(std::move(tls_map));
                // push poisson pill
                merge_queue.emplace_back(std::move(std::make_unique<std::map<std::string, size_t>>()));
                // push local map
                merge_queue.emplace_back(std::move(tls_map));
                status = false;
            } else if (merge_queue.size() > 1) {
                // pop from it
                auto new_map = std::move(merge_queue.front());
                // merge with local map
                std::for_each(new_map->begin(), new_map->end(),
                              [&](const std::map<std::string, size_t>::value_type &v) {
                                  tls_map->operator[](v.first) += v.second;
                              });
            }
        }
    }
}

#endif //INDEXING_MERGING_THREAD_WORKER_H
