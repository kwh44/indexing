
#include "indexing_thread_worker.hpp"
#include <boost/locale.hpp>
#include <boost/locale/boundary.hpp>

void index_worker(Mqueue<std::unique_ptr<std::string>> &index_queue,
                  Mqueue<std::unique_ptr<std::map<std::string, std::size_t>>> &merge_queue) {
    while (true) {
        auto string_to_index(std::move(index_queue.pop()));
        if (string_to_index->empty()) {
            index_queue.push(string_to_index);
            break;
        }
        auto tls_map = std::make_unique<std::map<std::string, size_t>>();
        boost::locale::boundary::ssegment_index map(boost::locale::boundary::word, string_to_index->begin(),
                                                    string_to_index->end());
        map.rule(boost::locale::boundary::word_any);
        for (auto v = map.begin(), end = map.end(); v != end; ++v) {
            ++tls_map->operator[](*v);
        }
        merge_queue.push(tls_map);
    }
}