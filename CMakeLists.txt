cmake_minimum_required(VERSION 3.16)

project(cs4223_cache_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/W4>"
		"$<$<CXX_COMPILER_ID:Clang,GNU>:-Wall;-Wextra;-Wpedantic>")
add_compile_definitions("$<$<CXX_COMPILER_ID:MSVC>:__cpp_lib_ranges>")
add_subdirectory(src)
