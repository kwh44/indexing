#ifndef PARALLEL_INDEXING_READ_FROM_ARCHIVE_H
#define PARALLEL_INDEXING_READ_FROM_ARCHIVE_H

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include "mqueue.hpp"
                        // queue of blocks, dir name, cv, m
void get_path_content(m_queue<std::string>&, std::string &, std::condition_variable&, std::mutex&);

#endif