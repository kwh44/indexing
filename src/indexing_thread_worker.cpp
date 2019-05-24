
#include "indexing_thread_worker.hpp"
#include <set>
#include <boost/locale.hpp>
#include <boost/locale/boundary.hpp>
#include <algorithm>


static void parse(std::string &v, std::vector<std::string> &tokens_list) {
    std::set<std::string> tokens_set;
    boost::locale::boundary::ssegment_index map(boost::locale::boundary::word, v.begin(), v.end());
    map.rule(boost::locale::boundary::word_any);
    tokens_set.insert(map.begin(), map.end());
    tokens_list.reserve(tokens_set.size());
    std::copy(tokens_set.begin(), tokens_set.end(), std::back_inserter(tokens_list));
}

static int count(std::string &str, const std::string &sub) {
    if (sub.length() == 0) return 0;
    int count = 0;
    for (size_t offset = str.find(sub); offset != std::string::npos;
         offset = str.find(sub, offset + sub.length())) {
        ++count;
    }
    return count;
}


static void token_usage(std::string &data, const std::vector<std::string> &token_list,
                        std::map<std::string, size_t> &tls_map) {
    size_t usage_count;
    for (const auto &token: token_list) {
        usage_count = count(data, token);
        tls_map.emplace(std::make_pair(token, usage_count));
    }
}


void index_worker(Mqueue<std::unique_ptr<std::string>> &index_queue,
                  Mqueue<std::unique_ptr<std::map<std::string, std::size_t>>> &merge_queue) {
    while (true) {
        auto string_to_index(std::move(index_queue.pop()));
        if (string_to_index->empty()) {
            index_queue.push(string_to_index);
            break;
        }
        auto tls_map = std::make_unique<std::map<std::string, size_t>>();
        std::vector<std::string> tokens;
        parse(*string_to_index, tokens);
        token_usage(*string_to_index, tokens, *tls_map);
        merge_queue.push(tls_map);
    }
}