#ifndef PARALLEL_INDEXING_COUNT_TOKEN_USAGE_H
#define PARALLEL_INDEXING_COUNT_TOKEN_USAGE_H

#include <vector>
#include <string>
#include <set>
#include <memory>
#include <map>
#include <mutex>

void token_usage(const std::string &, const std::vector<std::string> &,
                 std::map<std::string, size_t> &);

#endif //PARALLEL_INDEXING_COUNT_TOKEN_USAGE_H
