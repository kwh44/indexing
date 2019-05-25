#ifndef INDEXING_INDEXING_THREAD_WORKER_H
#define INDEXING_INDEXING_THREAD_WORKER_H

#include "mqueue.hpp"
#include <map>
#include <string>


void index_worker(Mqueue<std::unique_ptr<std::string>> &index_queue,
                  Mqueue<std::unique_ptr<std::map<std::string, std::size_t>>> &merge_queue);

#endif //INDEXING_INDEXING_THREAD_WORKER_H
