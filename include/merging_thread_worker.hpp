//
// Created by kwh44 on 5/21/19.
//

#ifndef INDEXING_MERGING_THREAD_WORKER_H
#define INDEXING_MERGING_THREAD_WORKER_H

#include <map>
#include <string>
#include "mqueue.hpp"


void merge_worker(Mqueue<std::unique_ptr<std::map<std::string, size_t>>> &merge_queue);

#endif //INDEXING_MERGING_THREAD_WORKER_H
