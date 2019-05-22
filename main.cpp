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

std::mutex io;


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
    m_queue<std::string> index_queue;
    std::mutex index_mtx;
    std::condition_variable index_cv;
    // run it in other thread
    std::thread reader(get_path_content, std::ref(index_queue), std::ref(conf_data.input_dir_name), std::ref(index_cv),
                       std::ref(index_mtx));


    m_queue<std::unique_ptr<std::map<std::string, size_t>>> merge_queue;
    std::mutex merge_mtx;
    std::condition_variable merge_cv;

    std::vector<std::thread> indexing_threads;
    indexing_threads.reserve(conf_data.indexing_thread_num);

    std::vector<std::thread> merging_threads;
    merging_threads.reserve(conf_data.merging_thread_num);

    for (size_t i = 0; i < conf_data.indexing_thread_num; ++i) {
        indexing_threads.emplace_back(index_worker, std::ref(index_queue), std::ref(index_cv), std::ref(index_mtx),
                std::ref(merge_queue), std::ref(merge_cv), std::ref(merge_mtx));
    }

    for (size_t i = 0; i < conf_data.merging_thread_num; ++i) {
        merging_threads.emplace_back(merge_worker, std::ref(merge_queue),
                std::ref(merge_mtx), std::ref(merge_cv));
    }
    reader.join();
    for (auto &v: indexing_threads) v.join();
    for (auto &v: merging_threads) v.join();

    std::cout << std::endl;
    std::cout << "Printing final map\n";
    auto final = std::move(merge_queue.front());
    for (auto it = final->begin(); it!=final->end(); ++it) {
        std::cout << it->first << " " << it->second << std::endl;
    }
    std::cout << "Merge_queue size is " << merge_queue.size() << std::endl;
    return 0;
}
