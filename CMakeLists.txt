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

# Добавляем директории с заголовочными файлами
include_directories(allocator/allocator)
include_directories(allocator/allocator_global_heap)
include_directories(be_tree)
include_directories(database)
include_directories(logger/client_logger)
include_directories(logger/logger)

# Сбор общих исходных файлов
file(GLOB_RECURSE COMMON_SOURCES
        allocator/allocator/*.cpp
        allocator/allocator/*.h
        allocator/allocator_global_heap/*.cpp
        allocator/allocator_global_heap/*.h
        be_tree/*.h
        database/*.h
        database/*.cpp
        logger/client_logger/*.cpp
        logger/client_logger/*.h
        logger/logger/*.cpp
        logger/logger/*.h
)

# Компиляция client.cpp
add_executable(client_program client.cpp ${COMMON_SOURCES})
target_link_libraries(client_program PRIVATE nlohmann_json::nlohmann_json pthread curl)

# Компиляция entrypoint.cpp
add_executable(entrypoint_program entrypoint.cpp ${COMMON_SOURCES})
target_link_libraries(entrypoint_program PRIVATE nlohmann_json::nlohmann_json pthread curl)

# Компиляция storage.cpp
add_executable(storage_program storage.cpp ${COMMON_SOURCES})
target_link_libraries(storage_program PRIVATE nlohmann_json::nlohmann_json pthread curl)
