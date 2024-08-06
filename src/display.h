#ifndef SPECTRUM_DISPLAY_H
#define SPECTRUM_DISPLAY_H

#include <stdint.h>

#define BORDER_SIZE 32
#define PAPER_WIDTH 256
#define PAPER_HEIGHT 192
#define DISPLAY_WIDTH (PAPER_WIDTH + BORDER_SIZE + BORDER_SIZE)
#define DISPLAY_HEIGHT (PAPER_HEIGHT + BORDER_SIZE + BORDER_SIZE)

/** Renders the contents of the given bank to the surface.
 * @param surface The destination surface in RGAB format.
 * @param bank The index of the source video memory bank.
 * @param border_attr Value for the border attribute.
 * @param invert_flash When non-zero, any flashing attribute blocks will be rendered
 * inverted.
 */
void display_render(uint32_t *surface,
                    uint8_t const *bank,
                    uint8_t border_attr,
                    int invert_flash);

#endif // SPECTRUM_DISPLAY_H
