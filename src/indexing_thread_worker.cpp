
#include "indexing_thread_worker.hpp"


void index_worker(Mqueue<std::string> &index_queue, Mqueue<std::map<std::string, std::size_t>> &merge_queue) {
    while (true) {
        std::string string_to_index(index_queue.pop());
        if (string_to_index.empty()) {
            index_queue.push(std::move(string_to_index));
//            finish work
            break;
        }
        std::map<std::string, size_t> tls_map;
        std::vector<std::string> tokens;
        parse(string_to_index, tokens);
        token_usage(string_to_index, tokens, tls_map);
        merge_queue.push(std::move(tls_map));
    }
}