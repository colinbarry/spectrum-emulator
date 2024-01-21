add_executable(Spectrum main.c spectrum.c keyboard.c display.c tape-loader.c memory.c tape-formats/tap-format.c tape-formats/tzx-format.c)
target_include_directories(Spectrum PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(Spectrum z80 ${SDL2_LIBRARIES})
target_compile_options(Spectrum PRIVATE -g)
