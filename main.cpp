#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>
#include <atomic>
#include <boost/locale/generator.hpp>
#include <boost/locale.hpp>
#include "read_config.hpp"
#include "mqueue.hpp"
#include "reading_thread.hpp"
#include "indexing_thread_worker.hpp"
#include "merging_thread_worker.hpp"

typedef std::pair<std::string, size_t> pair;


inline std::chrono::steady_clock::time_point get_current_time_fenced() {
    assert(std::chrono::steady_clock::is_steady &&
                   "Timer should be steady (monotonic).");
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::steady_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D &d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}


int main(int argc, char **argv) {
    // help info
    if (argc == 2 && std::string(argv[1]) == "--help") {
        std::cout << "Description\n" <<
                  "$ ./parallel_indexing <path_to_config_file>\n";
        return 0;
    }
    std::string filename("../config.dat");
    // user's config file
    if (argc == 2) {
        filename = std::string(argv[1]);
    }
    std::ifstream config_stream(filename);
    if (!config_stream.is_open()) {
        std::cerr << "Failed to open configuration file " << filename << std::endl;
        return 1;
    }
    config_data_t conf_data;
    try {
        read_config_data(config_stream, conf_data);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 2;
    }

    // check output files
    std::ofstream output_alphabet(conf_data.output_alphabet_order);
    if (!config_stream.is_open()) {
        std::cerr << "Failed to open file for alphabet order result" << std::endl;
        return 1;
    }
    std::ofstream output_count(conf_data.output_count_order);
    if (!config_stream.is_open()) {
        std::cerr << "Failed to open file for count order result " << std::endl;
        return 1;
    }

#ifdef DEBUG
    std::cout << "Input dir name \"" << conf_data.input_dir_name << "\"." << std::endl;
    std::cout << "Output alphabet order filename \"" << conf_data.output_alphabet_order << "\"." << std::endl;
    std::cout << "Output count order filename \"" << conf_data.output_count_order << "\"." << std::endl;
    std::cout << "Indexing threads num to utilize " << conf_data.indexing_thread_num << "." << std::endl;
    std::cout << "Merging threads num to utilize " << conf_data.merging_thread_num << "." << std::endl;
#endif

    boost::locale::generator gen;
    std::locale::global(gen("en_us.UTF-8"));

    Mqueue<std::unique_ptr<std::string>> index_queue;
    Mqueue<std::unique_ptr<std::map<std::string, size_t>>> merge_queue;

    auto start = get_current_time_fenced();
    std::thread reader(get_path_content, std::ref(index_queue), std::ref(conf_data.input_dir_name));

    std::vector<std::thread> indexing_threads;
    std::vector<std::thread> merging_threads;

    indexing_threads.reserve(conf_data.indexing_thread_num);
    merging_threads.reserve(conf_data.merging_thread_num);

    for (size_t i = 0; i < conf_data.indexing_thread_num; ++i) {
        indexing_threads.emplace_back(index_worker, std::ref(index_queue), std::ref(merge_queue));
    }
    for (size_t i = 0; i < conf_data.merging_thread_num; ++i) {
        merging_threads.emplace_back(merge_worker, std::ref(merge_queue));
    }
    reader.join();

#ifdef DEBUG
    std::cout << "READER JOINED" << std::endl;
#endif
    for (auto &v: indexing_threads) v.join();
    auto poisson_pill = std::make_unique<std::map<std::string, size_t>>();
    merge_queue.push(poisson_pill);
#ifdef DEBUG
    std::cout << "INDEXING JOINED" << std::endl;
#endif
    for (auto &v: merging_threads) v.join();

    auto total_finish = get_current_time_fenced();
    auto final_map = *std::move(merge_queue.pop());

    std::vector<std::pair<std::string, size_t>> sort_container(final_map.size());

    // copy key-value pairs from the map to the vector
    std::copy(final_map.begin(), final_map.end(), sort_container.begin());

    // sort the pair by alphabet
    std::sort(sort_container.begin(), sort_container.end(),
              [](const pair &l, const pair &r) {
                  return l.first < r.first;
              });
    // write to output_alphabet file
    for (auto &v: sort_container) {
        output_alphabet << v.first << ": " << v.second << std::endl;
    }

    // sort by usage count
    std::sort(sort_container.begin(), sort_container.end(),
              [](const pair &l, const pair &r) {
                  return l.second > r.second;
              });
    // write to output_count file
    for (auto &v: sort_container) {
        output_count << v.first << ": " << v.second << std::endl;
    }

    std::cout << "Total time is : " << to_us(total_finish - start) / 1000000.0 << std::endl;
    return 0;
}
