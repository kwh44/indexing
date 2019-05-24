#ifndef INDEXING_INDEXING_THREAD_WORKER_H
#define INDEXING_INDEXING_THREAD_WORKER_H

#include "mqueue.hpp"
#include <condition_variable>
#include <mutex>
#include <map>



void index_worker(Mqueue<std::string> &index_queue, Mqueue<std::map<std::string, std::size_t>> &merge_queue);

#endif //INDEXING_INDEXING_THREAD_WORKER_H
