//
// Created by kwh44 on 5/20/19.
//

#ifndef INDEXING_INDEXING_THREAD_WORKER_H
#define INDEXING_INDEXING_THREAD_WORKER_H

#include "mqueue.hpp"
#include <condition_variable>
#include <mutex>
#include <map>
#include "boundary_analysis.hpp"
#include "count_token_usage.hpp"

void index_worker(m_queue<std::string> &index_queue, std::condition_variable &index_cv, std::mutex &index_mtx,
                  m_queue<std::unique_ptr<std::map<std::string, size_t> > > &merge_queue,
                  std::condition_variable &merge_cv, std::mutex &merge_mtx) {

    bool status = true;

    std::string string_to_index;

    while (status == true) {

        auto tls_map = std::make_unique<std::map<std::string, size_t> >();

        std::unique_lock<std::mutex> lc(index_mtx);

        index_cv.wait(lc, [&index_queue]{return !index_queue.empty();});

        if (!index_queue.front().empty()) {

            string_to_index = std::move(index_queue.front());

            index_queue.pop();
        } else {

            status = false;

        }
        lc.unlock();

        if (status == true) {

            std::vector<std::string> tokens;

            parse(string_to_index, tokens);

            token_usage(string_to_index, tokens, tls_map);

        }

        {
            std::lock_guard<std::mutex> lk(merge_mtx);

            merge_queue.emplace_back(tls_map);
        }

        merge_cv.notify_one();

    }
    std::cout << "Index worker finished work\n";
}

#endif //INDEXING_INDEXING_THREAD_WORKER_H
