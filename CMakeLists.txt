cmake_minimum_required(VERSION 3.20)

project(cs4223_cache_sim
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
	add_compile_options(/Wall)
else()
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)
