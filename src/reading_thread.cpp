#include <archive.h>
#include <archive_entry.h>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include "reading_thread.hpp"

static void read_from_txt(std::ifstream &file, std::string &text) {
    auto ss = std::ostringstream{};
    ss << file.rdbuf();
    text = boost::locale::fold_case(boost::locale::normalize(ss.str()));
}

void get_path_content(Mqueue<std::string> &index_queue, std::string &dir_name) {
    std::vector<std::string> files_to_index;
    auto tt = boost::filesystem::recursive_directory_iterator(dir_name);
    unsigned read_files = 0;
    auto f = [&](boost::filesystem::recursive_directory_iterator &t) {
        for (; t != boost::filesystem::recursive_directory_iterator{} && read_files < 8; ++t) {
            boost::filesystem::path z(*t);
            if (boost::filesystem::is_directory(boost::filesystem::status(z))) continue;
            auto extension = boost::locale::fold_case(boost::locale::normalize(z.extension().string()));
            if (extension != ".zip" && extension != ".txt") continue;
            const auto &v = z.string();
            std::cout << "Reading " << v << std::endl;
            if (extension == ".txt") {
                std::string text;
                std::ifstream txt_file(v);
                if (txt_file.is_open()) {
                    read_from_txt(txt_file, text);
                    if (!text.empty()) {
                        index_queue.push(std::move(text));
                        ++read_files;
                    } else {
                        continue;
                    }
                } else {
                    continue;
                }
            } else {
                int response;
                ssize_t len;
                int64_t offset;
                const void *buff;
                size_t size;
                struct archive *a;
                struct archive_entry *entry;
                a = archive_read_new();
                archive_read_support_filter_all(a);
                archive_read_support_format_all(a);
                if ((response = archive_read_open_filename(a, v.c_str(), 10240))) {
                    std::cerr << "archive_read_open_filename() failed: " << archive_error_string(a) << std::endl;
                    continue;
                }
                while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
                    std::stringstream content;
                    response = archive_read_data_block(a, &buff, &size, &offset);
                    while (response != ARCHIVE_EOF && response == ARCHIVE_OK) {
                        char *buffer = static_cast<char *>(const_cast<void *>(buff));
                        content << boost::locale::fold_case(boost::locale::normalize(std::string(buffer, size)));
                        response = archive_read_data_block(a, &buff, &size, &offset);
                    }
                    if (!content.str().empty()) {
                        index_queue.push(content.str());
                        ++read_files;
                    } else {
                        continue;
                    }
                }
                archive_read_close(a);
                archive_read_free(a);
            }
        }
        read_files = 0;
    };
    while (tt != boost::filesystem::recursive_directory_iterator{}) {
        if (index_queue.size() > 5) continue;
        else f(tt);
    }
    index_queue.push("");
}
