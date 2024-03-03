find_package(SDL2 REQUIRED)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}" ${SDL2_INCLUDE_DIRS})

add_library(Keyboard OBJECT keyboard.c)
add_library(Display OBJECT display.c)
add_library(Memory OBJECT memory.c)
add_library(TapeLoader OBJECT tape-loader.c tape-formats/tap-format.c tape-formats/tzx-format.c)

option(SPECTRUM_BUILD_APP "Build the Spectrum emulator application" ON)
if (SPECTRUM_BUILD_APP)
    add_executable(Spectrum main.c spectrum.c)
    target_link_libraries(Spectrum z80 ${SDL2_LIBRARIES} Keyboard Display Memory TapeLoader)
    target_compile_features(Spectrum PUBLIC c_std_99)
endif()

option(SPECTRUM_BUILD_TESTS "Build the Spectrum tests" ON)
if (SPECTRUM_BUILD_TESTS)
    add_executable(Spectrum.test test-runner.c keyboard.test.c)
    target_compile_features(Spectrum.test PUBLIC c_std_99)
    target_link_libraries(Spectrum.test Keyboard picotest)
    add_test(NAME Spectrum.test COMMAND ./Spectrum.test)
endif()
