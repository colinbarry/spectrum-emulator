#ifndef SPECTRUM_DISPLAY_H
#define SPECTRUM_DISPLAY_H

#include <stdint.h>

#define BORDER_SIZE 32
#define PAPER_WIDTH 256
#define PAPER_HEIGHT 192
#define DISPLAY_WIDTH (PAPER_WIDTH + BORDER_SIZE + BORDER_SIZE)
#define DISPLAY_HEIGHT (PAPER_HEIGHT + BORDER_SIZE + BORDER_SIZE)

void display_render(uint32_t *surface,
                    uint8_t const *bank,
                    uint8_t border_attr,
                    int invert_flash);

#endif // SPECTRUM_DISPLAY_H
