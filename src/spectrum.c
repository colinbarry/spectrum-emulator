#include "spectrum.h"
#include "display.h"
#include "keyboard.h"
#include "memory.h"
#include "tape-loader.h"
#include <stdio.h>
#include <z80/z80.h>

static const int64_t clock_speed = 3500000;
static const int frames_per_second = 50;
#define CYCLES_PER_FRAME (clock_speed / frames_per_second)

static uint8_t mem_load(struct Z80* z80, uint16_t const addr)
{
    struct Spectrum* spectrum = (struct Spectrum*)(z80->userdata);
    return mem_read(spectrum->memory, addr);
}

static void mem_store(struct Z80* z80, uint16_t const addr, uint8_t const value)
{
    struct Spectrum* spectrum = (struct Spectrum*)(z80->userdata);
    mem_write(spectrum->memory, addr, value);
}

static uint8_t port_load(struct Z80* z80, uint16_t const port)
{
    struct Spectrum* spectrum = (struct Spectrum*)(z80->userdata);
    if ((port & 0x01) == 0)
    {
        return kb_read(spectrum->keyboard, port);
    }
    else
    {
        return 0xbf;
    }
}

static void port_store(struct Z80* z80, uint16_t port, uint8_t const val)
{
    struct Spectrum* spectrum = (struct Spectrum*)(z80->userdata);
    if ((port & 0x01) == 0)
    {
        spectrum->border_attr = val & 0x07;
    }
    else if (spectrum->model == s128 && (port & 0x02) == 0 && (port & 0x8000) == 0)
    {
        mem_page(spectrum->memory, val);
    }
}

static uint8_t trap(struct Z80* z80, uint16_t addr, uint8_t const opcode)
{
    if (addr == 0x056b && opcode == 0xc0)
    {
        struct Spectrum* spectrum = (struct Spectrum*)(z80->userdata);
        if (!spectrum->tape)
            return 1;

        uint8_t const block_type = z80->ap;
        uint16_t const addr = z80->ix;
        uint16_t const length = z80->de;

        int const success = tape_load_next_block(spectrum->tape,
                                                 block_type,
                                                 addr,
                                                 length,
                                                 spectrum->memory);

        // @TODO error handling!
        if (success)
        {
            z80->f |= 0x01; // @TODO for success
            z80->pc = 0x05e2;
        }
        else
        {
            puts("ERR");
            z80->f &= ~0x01;
        }

        return 1;
    }
    else
    {
        return 0;
    }
}

//=============================================================================

void spec_construct(struct Spectrum* self, enum Model const model)
{
    self->model = model;
    self->memory = malloc(sizeof(*self->memory));
    self->keyboard = malloc(sizeof(*self->keyboard));
    self->tape = NULL;
    self->z80 = malloc(sizeof(*self->z80));

    mem_construct(self->memory, model == s128);

    kb_construct(self->keyboard);

    self->cycles_until_interrupt = CYCLES_PER_FRAME;

    z80_init(self->z80);
    self->z80->userdata = self;
    self->z80->mem_load = &mem_load;
    self->z80->mem_store = &mem_store;
    self->z80->port_load = &port_load;
    self->z80->port_store = &port_store;
    self->z80->trap = &trap;
}

void spec_destruct(struct Spectrum* self)
{
    mem_destruct(self->memory);
    if (self->tape)
        tape_destroy(self->tape);
    free(self->z80);
    free(self->keyboard);
    free(self->memory);
}

bool spec_insert_tape(struct Spectrum* self, char const* filename)
{
    if (self->tape)
    {
        tape_destroy(self->tape);
        self->tape = NULL;
    }

    self->tape = tape_make(filename);
    return self->tape;
}

void spec_load_rom(struct Spectrum* self, int const page, char const* filename)
{
    FILE* romfile;
    int length;

    if ((romfile = fopen(filename, "rb")) == NULL)
    {
        printf("could not open rom file\n");
        exit(EXIT_FAILURE);
    }

    fseek(romfile, 0, SEEK_END);
    length = ftell(romfile);
    fseek(romfile, 0, SEEK_SET);

    uint8_t* buf = malloc(length);
    fread(buf, 1, length, romfile);
    mem_set_rom(self->memory, page & 0x01, buf, length);
    free(buf);

    fclose(romfile);
}

struct Keys
{
    int keys[2];
};

static struct Keys no_keys()
{
    struct Keys result = {-1, -1};
    return result;
}

static struct Keys key(int const key)
{
    struct Keys result = {key, -1};
    return result;
}

static struct Keys keys(int const first, int const second)
{
    struct Keys result = {first, second};
    return result;
}

/* Returns the key code for the given keypress, or -1 if there is no
 such key on the Spectrum
 */
static struct Keys convert_keycode(SDL_Keycode const keycode)
{
    switch (keycode)
    {
        case SDLK_0: return key(spk_0);
        case SDLK_1: return key(spk_1);
        case SDLK_2: return key(spk_2);
        case SDLK_3: return key(spk_3);
        case SDLK_4: return key(spk_4);
        case SDLK_5: return key(spk_5);
        case SDLK_6: return key(spk_6);
        case SDLK_7: return key(spk_7);
        case SDLK_8: return key(spk_8);
        case SDLK_9: return key(spk_9);
        case SDLK_a: return key(spk_a);
        case SDLK_b: return key(spk_b);
        case SDLK_c: return key(spk_c);
        case SDLK_d: return key(spk_d);
        case SDLK_e: return key(spk_e);
        case SDLK_f: return key(spk_f);
        case SDLK_g: return key(spk_g);
        case SDLK_h: return key(spk_h);
        case SDLK_i: return key(spk_i);
        case SDLK_j: return key(spk_j);
        case SDLK_k: return key(spk_k);
        case SDLK_l: return key(spk_l);
        case SDLK_m: return key(spk_m);
        case SDLK_n: return key(spk_n);
        case SDLK_o: return key(spk_o);
        case SDLK_p: return key(spk_p);
        case SDLK_q: return key(spk_q);
        case SDLK_r: return key(spk_r);
        case SDLK_s: return key(spk_s);
        case SDLK_t: return key(spk_t);
        case SDLK_u: return key(spk_u);
        case SDLK_v: return key(spk_v);
        case SDLK_w: return key(spk_w);
        case SDLK_x: return key(spk_x);
        case SDLK_y: return key(spk_y);
        case SDLK_z: return key(spk_z);
        case SDLK_RETURN: return key(spk_enter);
        case SDLK_SPACE: return key(spk_space);
        case SDLK_LSHIFT: return key(spk_caps_shift);
        case SDLK_LCTRL: return key(spk_symbol_shift);
        case SDLK_UP: return keys(spk_caps_shift, spk_7);
        case SDLK_DOWN: return keys(spk_caps_shift, spk_6);
        case SDLK_LEFT: return keys(spk_caps_shift, spk_5);
        case SDLK_RIGHT: return keys(spk_caps_shift, spk_8);
        case SDLK_DELETE:
        case SDLK_BACKSPACE: return keys(spk_caps_shift, spk_0);
        default: return no_keys();
    }
}

void spec_on_keydown(struct Spectrum* self, SDL_Keycode const key)
{
    struct Keys keys = convert_keycode(key);
    for (int i = 0; i < sizeof(keys.keys) / sizeof(int); ++i)
    {
        int const skey = keys.keys[i];
        if (skey != -1)
        {
            kb_on_keydown(self->keyboard, (enum SpectrumKey)skey);
        }
    }
}

void spec_on_keyup(struct Spectrum* self, SDL_Keycode const key)
{
    struct Keys keys = convert_keycode(key);
    for (int i = 0; i < sizeof(keys.keys) / sizeof(int); ++i)
    {
        int const skey = keys.keys[i];
        if (skey != -1)
        {
            kb_on_keyup(self->keyboard, (enum SpectrumKey)skey);
        }
    }
}

void spec_run(struct Spectrum* self, int cycles)
{
    while (cycles > 0)
    {
        int const step_cycles = z80_step(self->z80);
        cycles -= step_cycles;
        self->cycles_until_interrupt -= step_cycles;

        if (self->cycles_until_interrupt <= 0)
        {
            z80_interrupt(self->z80, 0);
            self->cycles_until_interrupt += CYCLES_PER_FRAME;
        }
    }
}

void spec_render_display(struct Spectrum* self, uint32_t* surface)
{
    display_render(surface,
                   mem_screen(self->memory),
                   self->border_attr,
                   self->frame >= 16);
    self->frame = (self->frame + 1) % 32;
}

int spec_cycles(float const seconds)
{
    return (int)(seconds * clock_speed);
}
