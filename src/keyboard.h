#ifndef SPECTRUM_KEYBOARD_H
#define SPECTRUM_KEYBOARD_H

#include "SDL.h"
#include <stdbool.h>
#include <stdint.h>

enum SpectrumKey
{
    spk_1,
    spk_2,
    spk_3,
    spk_4,
    spk_5,
    spk_6,
    spk_7,
    spk_8,
    spk_9,
    spk_0,
    spk_q,
    spk_w,
    spk_e,
    spk_r,
    spk_t,
    spk_y,
    spk_u,
    spk_i,
    spk_o,
    spk_p,
    spk_a,
    spk_s,
    spk_d,
    spk_f,
    spk_g,
    spk_h,
    spk_j,
    spk_k,
    spk_l,
    spk_enter,
    spk_caps_shift,
    spk_z,
    spk_x,
    spk_c,
    spk_v,
    spk_b,
    spk_n,
    spk_m,
    spk_symbol_shift,
    spk_space
};

struct Keyboard
{
    bool keys[40];
};

void kb_construct(struct Keyboard *self);

uint8_t kb_read(struct Keyboard const *self, uint16_t port);

void kb_on_keydown(struct Keyboard *self, enum SpectrumKey key);
void kb_on_keyup(struct Keyboard *self, enum SpectrumKey key);

#endif // SPECTRUM_KEYBOARD_H
