#include "keyboard.h"

static uint8_t pack_keypresses(struct Keyboard const *keyboard,
                               int k0,
                               int k1,
                               int k2,
                               int k3,
                               int k4)
{
    return ~(keyboard->keys[k0] | (keyboard->keys[k1] << 1)
             | (keyboard->keys[k2] << 2) | (keyboard->keys[k3] << 3)
             | (keyboard->keys[k4] << 4));
}

//=============================================================================

void kb_construct(struct Keyboard *self)
{
    memset(self, 0, sizeof(*self));
}

uint8_t kb_read(struct Keyboard const *self, uint16_t port)
{
    uint8_t const row = port >> 8;
    uint8_t result = 0xff;
    if (~row & 0x01)
    {
        result &= pack_keypresses(self, spk_caps_shift, spk_z, spk_x, spk_c, spk_v);
    }
    if (~row & 0x02)
    {
        result &= pack_keypresses(self, spk_a, spk_s, spk_d, spk_f, spk_g);
    }
    if (~row & 0x04)
    {
        result &= pack_keypresses(self, spk_q, spk_w, spk_e, spk_r, spk_t);
    }
    if (~row & 0x08)
    {
        result &= pack_keypresses(self, spk_1, spk_2, spk_3, spk_4, spk_5);
    }
    if (~row & 0x10)
    {
        result &= pack_keypresses(self, spk_0, spk_9, spk_8, spk_7, spk_6);
    }
    if (~row & 0x20)
    {
        result &= pack_keypresses(self, spk_p, spk_o, spk_i, spk_u, spk_y);
    }
    if (~row & 0x40)
    {
        result &= pack_keypresses(self, spk_enter, spk_l, spk_k, spk_j, spk_h);
    }
    if (~row & 0x80)
    {
        result &= pack_keypresses(self, spk_space, spk_symbol_shift, spk_m, spk_n, spk_b);
    }

    return result;
}

void kb_on_keydown(struct Keyboard *self, enum SpectrumKey const key)
{
    self->keys[key] = true;
}

void kb_on_keyup(struct Keyboard *self, enum SpectrumKey const key)
{
    self->keys[key] = false;
}
