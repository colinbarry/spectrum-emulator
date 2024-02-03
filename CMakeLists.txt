cmake_minimum_required(VERSION 3.14)

project(Spectrum)

include(FetchContent)
FetchContent_Declare(
    z80
    GIT_REPOSITORY https://github.com/colinbarry/z80 
    GIT_TAG main)
FetchContent_MakeAvailable(z80)

add_subdirectory(src ./bin)
