#ifndef SPECTRUM_KEYBOARD_H
#define SPECTRUM_KEYBOARD_H

#include "SDL.h"
#include <stdbool.h>
#include <stdint.h>

/** Enumeration of keys on the (48k) Spectrum keyboard.
 */
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

/** State of the keyboard keys.
 */
struct Keyboard
{
    bool keys[40];
};

/** Initializes a Keyboard struct.
 */
void kb_construct(struct Keyboard *self);

/** Returns the 8-bit key values representing the rows of keys in the given
 * port.
 * @param self The keyboard
 * @param port The port corresponding to the keyboard row.
 * @return Bitmask representing which keys in the row are depressed.
 */
uint8_t kb_read(struct Keyboard const *self, uint16_t port);

/** Flags the given key as depressed.
 */
void kb_on_keydown(struct Keyboard *self, enum SpectrumKey key);
/** Flags the given key as released.
 */
void kb_on_keyup(struct Keyboard *self, enum SpectrumKey key);

#endif // SPECTRUM_KEYBOARD_H
