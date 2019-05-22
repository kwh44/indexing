#include "count_token_usage.hpp"
#include <iostream>
#include <boost/locale.hpp>
#include <boost/locale/boundary.hpp>
#include <algorithm>
#include <set>
#include "boundary_analysis.hpp"

static int count(const std::string &str, const std::string &sub) {
    if (sub.length() == 0) return 0;
    int count = 0;
    for (size_t offset = str.find(sub); offset != std::string::npos;
         offset = str.find(sub, offset + sub.length())) {
        ++count;
    }
    return count;
}


void token_usage(const std::string &data, const std::vector<std::string> &token_list,
                 std::map<std::string, size_t>  &tls_map) {
    size_t usage_count;
    for (const auto &token: token_list) {
        usage_count = count(data, token);
        tls_map.emplace(std::make_pair(token, usage_count));
    }
}
