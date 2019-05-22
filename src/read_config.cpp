#include <iostream>
#include <algorithm>
#include "read_config.hpp"

void read_config_data(std::ifstream &file, config_data_t &container) {
    auto container_copy(container); // save container state
    std::ios::fmtflags flags(file.flags()); // Save stream state
    file.exceptions(std::ifstream::failbit); // Enable exception on fail
    try {
        std::string temp;
        file >> temp;
        container.input_dir_name = std::string(std::find(temp.begin(), temp.end(), '"') + 1, temp.end() - 1);
        file >> temp;
        container.output_alphabet_order = std::string(std::find(temp.begin(), temp.end(), '"') + 1, temp.end() - 1);
        file >> temp;
        container.output_count_order = std::string(std::find(temp.begin(), temp.end(), '"') + 1, temp.end() - 1);
        file >> temp;
        container.indexing_thread_num = std::stol(
                std::string(std::find(temp.begin(), temp.end(), '=') + 1, temp.end()));
        file >> temp;
        container.merging_thread_num = std::stol(std::string(std::find(temp.begin(), temp.end(), '=') + 1, temp.end()));
    } catch (...) {
        container = container_copy;
        file.flags(flags);
        throw;
    }
}