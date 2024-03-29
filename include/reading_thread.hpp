#ifndef PARALLEL_INDEXING_READ_FROM_ARCHIVE_H
#define PARALLEL_INDEXING_READ_FROM_ARCHIVE_H


#include <string>
#include "mqueue.hpp"


void get_path_content(Mqueue<std::unique_ptr<std::string>> &, std::string &);

#endif