#ifndef SPECTRUM_DISPLAY_H
#define SPECTRUM_DISPLAY_H

#include <stdint.h>

static const int border_size = 32;
static const int paper_width = 256;
static const int paper_height = 192;
static const int display_width = paper_width + border_size + border_size;
static const int display_height = paper_height + border_size + border_size;

void display_render(uint32_t* surface,
                    uint8_t const* bank,
                    uint8_t border_attr,
                    int invert_flash);

#endif // SPECTRUM_DISPLAY_H
