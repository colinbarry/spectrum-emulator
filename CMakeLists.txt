cmake_minimum_required(VERSION 3.14)

project(Spectrum)

include(FetchContent)
FetchContent_Declare(
    z80
    GIT_REPOSITORY https://github.com/colinbarry/z80 
    GIT_TAG main)
FetchContent_MakeAvailable(z80)

option(ENABLE_TESTING "Enable the tests" ON)
if (ENABLE_TESTING AND CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    FetchContent_Declare(
        Picotest
        GIT_REPOSITORY https://github.com/colinbarry/picotest
        GIT_TAG main)
    FetchContent_MakeAvailable(Picotest)

    enable_testing()
endif()

add_subdirectory(src ./bin)
