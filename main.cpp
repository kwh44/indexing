#include <iostream>
#include <thread>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <mutex>
#include <numeric>
#include <boost/locale/generator.hpp>
#include <boost/locale.hpp>
#include <condition_variable>
#include "read_config.hpp"
#include "measure_time.hpp"
#include "read_from_path.hpp"
#include "boundary_analysis.hpp"
#include "count_token_usage.hpp"
#include "mqueue.hpp"
#include "indexing_thread_worker.hpp"
#include "merging_thread_worker.hpp"

typedef std::pair<std::string, size_t> pair;

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

    // array of blocks
    Mqueue<std::string> index_queue;
    Mqueue<std::map<std::string, size_t>> merge_queue;

    std::mutex index_mtx;
//    // run it in other thread
	auto start = get_current_time_fenced();
    std::thread reader(get_path_content, std::ref(index_queue), std::ref(conf_data.input_dir_name));


//
    std::vector<std::thread> indexing_threads;
    indexing_threads.reserve(conf_data.indexing_thread_num);
//
    std::vector<std::thread> merging_threads;
    merging_threads.reserve(conf_data.merging_thread_num);
//
    for (size_t i = 0; i < conf_data.indexing_thread_num; ++i) {
        indexing_threads.emplace_back(index_worker, std::ref(index_queue), std::ref(merge_queue));
    }
    for (size_t i = 0; i < conf_data.merging_thread_num; ++i) {
        merging_threads.emplace_back(merge_worker, std::ref(merge_queue));
    }
    reader.join();
    std::cout << "READER JOINED" << std::endl;
    for (auto &v: indexing_threads) v.join();
    std::map<std::string, size_t> merge_queue_empty;
    merge_queue.push(std::move(merge_queue_empty));
    std::cout << "INDEXING JOINED" << std::endl;
    for (auto &v: merging_threads) v.join();
	auto total_finish = get_current_time_fenced();
    auto final_map = merge_queue.pop();

    std::vector<std::pair<std::string, size_t>> sort_container(final_map.size());

    // copy key-value pairs from the map to the vector
    std::copy(final_map.begin(), final_map.end(), sort_container.begin());

    // sort the pair by alphabet
    std::sort(sort_container.begin(), sort_container.end(),
              [](const pair &l, const pair &r) {
                  return l.first < r.first;
              });
    // write to output file
    for (auto &v: sort_container) {
        output_alphabet << v.first << ": " << v.second << std::endl;
    }
    // sort by usage count
    std::sort(sort_container.begin(), sort_container.end(),
              [](const pair &l, const pair &r) {
                  return l.second > r.second;
              });
    // write to output file
    for (auto &v: sort_container) {
        output_count << v.first << ": " << v.second << std::endl;
    }
    std::cout << "Total time is : " << to_us(total_finish - start) / 1000000.0 << std::endl;
    return 0;
}
