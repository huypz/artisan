cmake_minimum_required(VERSION 3.17)
project(artisan LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(godot-cpp)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(artisan SHARED ${SOURCES})

target_link_libraries(artisan PRIVATE godot-cpp)

set_target_properties(artisan PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/project/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/project/bin"
)
