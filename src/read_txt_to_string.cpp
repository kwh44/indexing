#include "read_txt_to_string.hpp"
#include <limits>
#include <boost/locale.hpp>

void read_from_txt(std::ifstream &file, std::string &text) {
    auto ss = std::ostringstream{};
    ss << file.rdbuf();
    text = boost::locale::fold_case(boost::locale::normalize(ss.str()));
}