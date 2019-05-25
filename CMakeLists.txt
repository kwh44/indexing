cmake_minimum_required(VERSION 3.0)

project(indexing)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE Release)

find_package(Boost COMPONENTS locale system filesystem REQUIRED)

find_package(Threads REQUIRED)

find_package(LibArchive REQUIRED)

include_directories(${LibArchive_INCLUDE_DIRS})

# add_compile_options("-DDEBUG")

set(Boost_USE_STATIC_LIBS OFF)

set(Boost_USE_MULTITHREAD ON)

include_directories(include)
include_directories(Boost::filesystem)
include_directories(Boost::locale)

add_executable(indexing main.cpp
        include/read_config.hpp src/read_config.cpp
        include/reading_thread.hpp src/reading_thread.cpp
        include/mqueue.hpp
        include/indexing_thread_worker.hpp src/indexing_thread_worker.cpp
        include/merging_thread_worker.hpp src/merging_thread_worker.cpp)

target_link_libraries(indexing Boost::locale Boost::filesystem ${LibArchive_LIBRARIES} Threads::Threads)
