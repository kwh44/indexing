#include <archive.h>
#include <archive_entry.h>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include <boost/algorithm/string.hpp>
#include "reading_thread.hpp"

static void read_from_txt(std::ifstream &file, std::string &text) {
    auto ss = std::ostringstream{};
    ss << file.rdbuf();
    text = boost::locale::fold_case(boost::locale::normalize(ss.str()));
}

void get_path_content(Mqueue<std::unique_ptr<std::string>> &index_queue, std::string &dir_name) {
    std::vector<std::string> files_to_index;
    auto t = boost::filesystem::recursive_directory_iterator(dir_name);
    int read_files = 0;
    for (; t != boost::filesystem::recursive_directory_iterator{}; ++t) {
        boost::filesystem::path z(*t);
        if (boost::filesystem::is_directory(boost::filesystem::status(z))) continue;
        auto extension = boost::locale::fold_case(boost::locale::normalize(z.extension().string()));
        if (extension != ".zip" && extension != ".txt") continue;
        const auto &v = z.string();
#ifdef DEBUG
        std::cout << "Reading " << v << std::endl;
#endif
        if (extension == ".txt") {
            auto text = std::make_unique<std::string>();
            std::ifstream txt_file(v);
            if (!txt_file.is_open()) continue;
            read_from_txt(txt_file, *text);
            if (text->empty())continue;
            index_queue.push(text);
            ++read_files;
        } else {
            struct archive *a;
            struct archive_entry *entry;
            a = archive_read_new();
            archive_read_support_filter_all(a);
            archive_read_support_format_all(a);
            if (archive_read_open_filename(a, v.c_str(), 10240)) {
                std::cerr << "archive_read_open_filename() failed: " << archive_error_string(a) << std::endl;
                continue;
            }
            while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
                auto content = std::make_unique<std::string>();
                const char *path = archive_entry_pathname(entry);
                if (boost::ends_with(boost::locale::fold_case(std::string(path)), ".txt")) {
                    auto entry_size = archive_entry_size(entry);
                    if (entry_size <= 0) continue;
                    std::string read_to(entry_size, ' ');
                    archive_read_data(a, &read_to[0], entry_size);
                    *content = boost::locale::normalize(boost::locale::fold_case(read_to));
                    if (content->empty()) continue;
                    index_queue.push(content);
                    ++read_files;
                }
            }
            archive_read_close(a);
            archive_read_free(a);
        }
        if (read_files > 500) {
            index_queue.continue_read();
            read_files = 0;
        }
    }
    auto poisson_pill = std::make_unique<std::string>();
    index_queue.push(poisson_pill);
}
