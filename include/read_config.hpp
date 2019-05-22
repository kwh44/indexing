#ifndef READ_CONFIG_FILE_PARALLEL_INDEXING
#define READ_CONFIG_FILE_PARALLEL_INDEXING

#include <fstream>
#include <string>

struct config_data_t {
    std::string input_dir_name;
    std::string output_alphabet_order;
    std::string output_count_order;
    size_t indexing_thread_num;
    size_t merging_thread_num;
};


void read_config_data(std::ifstream &, struct config_data_t &);

#endif // READ_CONFIG_FILE_PARALLEL_INDEXING

