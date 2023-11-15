#include "keyboard.h"

enum Keys
{
    skey_a,
    skey_b,
    skey_c,
    skey_d,
    skey_e,
    skey_f,
    skey_g,
    skey_h,
    skey_i,
    skey_j,
    skey_k,
    skey_l,
    skey_m,
    skey_n,
    skey_o,
    skey_p,
    skey_q,
    skey_r,
    skey_s,
    skey_t,
    skey_u,
    skey_v,
    skey_w,
    skey_x,
    skey_y,
    skey_z,
    skey_0,
    skey_1,
    skey_2,
    skey_3,
    skey_4,
    skey_5,
    skey_6,
    skey_7,
    skey_8,
    skey_9,
    skey_shift,
    skey_sym,
    skey_space,
    skey_enter
};

static int keycode_to_skey(SDL_Keycode const keycode)
{
    switch (keycode)
    {
        case SDLK_0: return skey_0;
        case SDLK_1: return skey_1;
        case SDLK_2: return skey_2;
        case SDLK_3: return skey_3;
        case SDLK_4: return skey_4;
        case SDLK_5: return skey_5;
        case SDLK_6: return skey_6;
        case SDLK_7: return skey_7;
        case SDLK_8: return skey_8;
        case SDLK_9: return skey_9;
        case SDLK_a: return skey_a;
        case SDLK_b: return skey_b;
        case SDLK_c: return skey_c;
        case SDLK_d: return skey_d;
        case SDLK_e: return skey_e;
        case SDLK_f: return skey_f;
        case SDLK_g: return skey_g;
        case SDLK_h: return skey_h;
        case SDLK_i: return skey_i;
        case SDLK_j: return skey_j;
        case SDLK_k: return skey_k;
        case SDLK_l: return skey_l;
        case SDLK_m: return skey_m;
        case SDLK_n: return skey_n;
        case SDLK_o: return skey_o;
        case SDLK_p: return skey_p;
        case SDLK_q: return skey_q;
        case SDLK_r: return skey_r;
        case SDLK_s: return skey_s;
        case SDLK_t: return skey_t;
        case SDLK_u: return skey_u;
        case SDLK_v: return skey_v;
        case SDLK_w: return skey_w;
        case SDLK_x: return skey_x;
        case SDLK_y: return skey_y;
        case SDLK_z: return skey_z;
        case SDLK_RETURN: return skey_enter;
        case SDLK_SPACE: return skey_space;
        case SDLK_LSHIFT: return skey_shift;
        case SDLK_LCTRL: return skey_sym;
        default: return -1;
    }
}

static uint8_t pack_keypresses(struct Keyboard const* keyboard,
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

void kb_construct(struct Keyboard* self)
{
    memset(self, 0, sizeof(*self));
}

uint8_t kb_read(struct Keyboard const* self, uint16_t port)
{
    uint8_t const row = port >> 8;
    uint8_t result = 0xff;
    if (~row & 0x01)
    {
        result &= pack_keypresses(self, skey_shift, skey_z, skey_x, skey_c, skey_v);
    }
    if (~row & 0x02)
    {
        result &= pack_keypresses(self, skey_a, skey_s, skey_d, skey_f, skey_g);
    }
    if (~row & 0x04)
    {
        result &= pack_keypresses(self, skey_q, skey_w, skey_e, skey_r, skey_t);
    }
    if (~row & 0x08)
    {
        result &= pack_keypresses(self, skey_1, skey_2, skey_3, skey_4, skey_5);
    }
    if (~row & 0x10)
    {
        result &= pack_keypresses(self, skey_0, skey_9, skey_8, skey_7, skey_6);
    }
    if (~row & 0x20)
    {
        result &= pack_keypresses(self, skey_p, skey_o, skey_i, skey_u, skey_y);
    }
    if (~row & 0x40)
    {
        result &= pack_keypresses(self, skey_enter, skey_l, skey_k, skey_j, skey_h);
    }
    if (~row & 0x80)
    {
        result &= pack_keypresses(self, skey_space, skey_sym, skey_m, skey_n, skey_b);
    }

    return result;
}

void kb_on_keydown(struct Keyboard* self, SDL_Keycode const key)
{
    int const skey = keycode_to_skey(key);
    if (skey != -1)
        self->keys[skey] = 1;
}

void kb_on_keyup(struct Keyboard* self, SDL_Keycode const key)
{
    int const skey = keycode_to_skey(key);
    if (skey != -1)
        self->keys[skey] = 0;
}
