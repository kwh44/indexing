#include <archive.h>
#include <archive_entry.h>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include "read_txt_to_string.hpp"
#include "read_from_path.hpp"

void get_path_content(Mqueue<std::string> &index_queue, std::string &dir_name) {
    std::vector<std::string> files_to_index;
    for (auto t = boost::filesystem::recursive_directory_iterator(dir_name);
         t != boost::filesystem::recursive_directory_iterator{}; ++t) {
        boost::filesystem::path z(*t);
        if (boost::filesystem::is_directory(boost::filesystem::status(z))) continue;
        auto extension = boost::locale::fold_case(boost::locale::normalize(z.extension().string()));
        if (extension != ".zip" && extension != ".txt") continue;
        const auto &v = z.string();
        if (extension == ".txt") {
            std::string text;
            std::ifstream txt_file(v);
            if (txt_file.is_open()) {
                read_from_txt(txt_file, text);
                index_queue.push(text);
            } else {
                continue;
            }
        } else {

            int response;
            ssize_t len;
            constexpr size_t buffer_size = 8192;
            char buff[buffer_size];
            struct archive *a;
            struct archive_entry *entry;
            a = archive_read_new();
            archive_read_support_filter_all(a);
            archive_read_support_format_all(a);
            if ((response = archive_read_open_filename(a, v.c_str(), buffer_size))) {
                std::cerr << "archive_read_open_filename() failed: " << archive_error_string(a) << std::endl;
                continue;
            }
            while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
                if (archive_entry_size(entry) > 0) {
                    std::stringstream entry_content_string;
                    len = archive_read_data(a, buff, sizeof(buff));
                    while (len > 0) {
                        entry_content_string << boost::locale::fold_case(boost::locale::normalize(std::string(buff)));
                        len = archive_read_data(a, buff, sizeof(buff));
                    }
                    {
                        index_queue.push(entry_content_string.str());
                    }
                }
            }
            archive_read_close(a);
            archive_read_free(a);
        }
    }
    {
        //push poison pill
        index_queue.push("");
    }
    std::cout << "Reader thread finished work\n";
}


