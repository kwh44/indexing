#include "merging_thread_worker.hpp"
void merge_worker(Mqueue<std::map<std::string, size_t>> &merge_queue) {
    std::map<std::string, size_t> tls_map;
    while (true) {
        auto single_dict(merge_queue.pop());
        if (single_dict.empty()) {
            if (!tls_map.empty()) {
                merge_queue.push(std::move(tls_map));
            }
            merge_queue.push(std::move(single_dict));
            break;
        }
        std::for_each(single_dict.begin(), single_dict.end(),
                      [&](const std::map<std::string, size_t>::value_type &v) {
                          tls_map.operator[](v.first) += v.second;
                      });
    }
}