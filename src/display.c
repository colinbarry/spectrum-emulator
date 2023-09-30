#include "display.h"

static uint32_t colour(uint8_t const attr)
{
    uint8_t const g = (attr & 0x04) ? 0xee : 0x00;
    uint8_t const r = (attr & 0x02) ? 0xee : 0x00;
    uint8_t const b = (attr & 0x01) ? 0xee : 0x00;
    return r | (g << 8) | (b << 16) | (0xff << 24);
}

static void fill_rect(uint32_t *pixels, uint32_t const colour, int x, int y, int w, int h)
{
    uint32_t* wr = pixels + y * display_width + x;

    for (int r = 0; r < h; ++r) {
        for (int c = 0; c < w; ++c) {
            *wr++ = colour;
       }

        wr += display_width - w;
    }
}

//=============================================================================

void display_render(uint32_t* surface, uint8_t const* memory, uint8_t border_attr, int invert_flash)
{
    uint32_t const brgb = colour(border_attr);

    fill_rect(surface, brgb, 0, 0, display_width, border_size);
    fill_rect(surface, brgb, 0, display_height - border_size, display_width, border_size);
    fill_rect(surface, brgb, 0, border_size, border_size, display_height - border_size - border_size);
    fill_rect(surface, brgb, display_width - border_size, border_size, border_size, display_height - border_size - border_size);

    for (int r = 0; r < 24; ++r) {
        for (int c = 0; c < 32; ++c) {
            uint32_t* wr = surface + display_width * border_size + border_size + r * (display_width * 8) + c * 8;
            uint8_t const attr = memory[0x5800 + r * 0x20 + c];

            uint32_t irgb = colour(attr);
            uint32_t prgb = colour(attr >> 3);

            if (attr & (1 << 7) && invert_flash) {
                uint32_t tmp = irgb;
                irgb = prgb;
                prgb = tmp;
            }

            uint16_t addr;
            if (r < 8) {
                addr = 0x4000 + r * 0x20 + c;
            } else if (r < 16) {
                addr = 0x4800 + (r % 8) * 0x20 + c;
            } else {
                addr = 0x5000 + (r % 8) * 0x20 + c;
            }

            for (int i = 0; i < 8; ++i) {
               uint8_t byte = memory[addr];

                for (int j = 0; j < 8; ++j) {
                    *wr++ = (byte & 0x80) ? irgb : prgb;
                    byte <<= 1;
                }

                addr += 0x100;
                wr += display_width - 8;
            }
        }
    }
}
