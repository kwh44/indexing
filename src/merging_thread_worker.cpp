#include "merging_thread_worker.hpp"
#include <algorithm>

void merge_worker(Mqueue<std::unique_ptr<std::map<std::string, size_t>>> &merge_queue) {
    auto tls_map = std::make_unique<std::map<std::string, size_t>>();
    while (true) {
        auto single_dict(std::move(merge_queue.pop()));
        if (single_dict->empty()) {
            if (!tls_map->empty()) {
                merge_queue.push(tls_map);
            }
            merge_queue.push(single_dict);
            break;
        }
        std::for_each(single_dict->begin(), single_dict->end(),
                      [&](const std::map<std::string, size_t>::value_type &v) {
                          tls_map->operator[](v.first) += v.second;
                      });
    }
}
