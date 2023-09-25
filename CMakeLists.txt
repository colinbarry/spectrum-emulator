cmake_minimum_required(VERSION 3.20)

project(Spectrum)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(src ./bin)
add_subdirectory(extern/z80)
