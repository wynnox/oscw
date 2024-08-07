cmake_minimum_required(VERSION 3.22)
project(course_project)

set(CMAKE_CXX_STANDARD 17)

# Включение модуля FetchContent
include(FetchContent)

# Определение FetchContent для json
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

include_directories(allocator/allocator)
include_directories(allocator/allocator_global_heap)
include_directories(be_tree)
include_directories(data_collections)
include_directories(logger/client_logger)
include_directories(logger/logger)

file(GLOB_RECURSE SOURCES
        main.cpp
        allocator/allocator/*.cpp
        allocator/allocator/*.h
        allocator/allocator_global_heap/*.cpp
        allocator/allocator_global_heap/*.h
        be_tree/*.h
        data_collections/*.h
        logger/client_logger/*.cpp
        logger/client_logger/*.h
        logger/logger/*.cpp
        logger/logger/*.h
)

add_executable(course_project ${SOURCES}
        allocator/allocator_global_heap/typename_holder.h
        data_collections/data.h)

target_link_libraries(course_project PRIVATE nlohmann_json::nlohmann_json)
