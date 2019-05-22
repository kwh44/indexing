#include <set>
#include <boost/locale.hpp>
#include <boost/locale/boundary.hpp>
#include "boundary_analysis.hpp"

void parse(std::string &v, std::vector<std::string> &tokens_list) {
    /*
     * Input: vector of strings (v)
     * stores unique words in (tokens_list) from (data).
     */

    std::set<std::string> tokens_set;
    boost::locale::boundary::ssegment_index map(boost::locale::boundary::word, v.begin(), v.end());
    map.rule(boost::locale::boundary::word_any);
    tokens_set.insert(map.begin(), map.end());
    tokens_list.reserve(tokens_set.size());
    std::copy(tokens_set.begin(), tokens_set.end(), std::back_inserter(tokens_list));
}
