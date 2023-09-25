add_executable(Spectrum main.c)
target_link_libraries(Spectrum z80 ${SDL2_LIBRARIES})
target_compile_options(Spectrum PRIVATE -g)
