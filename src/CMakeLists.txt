add_executable(Spectrum main.c spectrum.c keyboard.c display.c tap-loader.c memory.c)
target_include_directories(Spectrum PRIVATE spectrum.h keyboard.h display.h tap-loader.h memory.h)
target_link_libraries(Spectrum z80 ${SDL2_LIBRARIES})
target_compile_options(Spectrum PRIVATE -g)
