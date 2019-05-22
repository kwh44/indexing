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

void merge_worker(m_queue<std::map<std::string, size_t>> &merge_queue) {
    std::map<std::string, size_t> tls_map;
    while (true) {
        auto single_dict = merge_queue.pop();
        if (single_dict.empty()) {
            if (!tls_map.empty()) {
                std::cout << "empty" << std::endl;
                merge_queue.push(std::move(tls_map));

            }
            merge_queue.push(std::move(single_dict));
            break;

        }
        std::for_each(single_dict.begin(), single_dict.end(),
                      [&](const std::map<std::string, size_t>::value_type &v) {
                          tls_map.operator[](v.first) += v.second;
                      });
    }

}

#endif //INDEXING_MERGING_THREAD_WORKER_H
