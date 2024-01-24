cmake_minimum_required(VERSION 3.20)

project(Spectrum)

include(FetchContent)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

FetchContent_Declare(
    z80
    GIT_REPOSITORY https://github.com/colinbarry/z80 
    GIT_TAG main)

FetchContent_MakeAvailable(z80)

add_subdirectory(src ./bin)
