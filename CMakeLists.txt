cmake_minimum_required(VERSION 3.0)

project(indexing)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE Release)

find_package(Boost COMPONENTS locale system filesystem REQUIRED)

find_package(Threads REQUIRED)

find_package(LibArchive REQUIRED)

find_package(ICU COMPONENTS data i18n io test tu REQUIRED)

add_compile_options("-DDEBUG")

set(Boost_USE_STATIC_LIBS OFF)

set(Boost_USE_MULTITHREAD ON)

include_directories(include)

add_executable(indexing main.cpp
        include/read_config.hpp src/read_config.cpp
        include/measure_time.hpp
        include/read_from_path.hpp src/read_from_path.cpp
        include/boundary_analysis.hpp src/boundary_analysis.cpp
        include/read_txt_to_string.hpp src/read_txt_to_string.cpp
        include/count_token_usage.hpp src/count_token_usage.cpp
        include/mqueue.hpp src/mqueue.cpp
        include/indexing_thread_worker.hpp src/indexing_thread_worker.cpp
        include/merging_thread_worker.hpp src/merging_thread_worker.cpp)

target_link_libraries(indexing Boost::locale Boost::filesystem archive Threads::Threads)