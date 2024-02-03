#include "spectrum.h"
#include "display.h"
#include "keyboard.h"
#include "memory.h"
#include "tape-loader.h"
#include <stdio.h>
#include <z80/z80.h>

static const int64_t clock_speed = 3500000;
static const int frames_per_second = 50;
static const int cycles_per_frame = clock_speed / frames_per_second;

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

    self->cycles_until_interrupt = cycles_per_frame;

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

void spec_on_keydown(struct Spectrum* self, SDL_Keycode const key)
{
    kb_on_keydown(self->keyboard, key);
}

void spec_on_keyup(struct Spectrum* self, SDL_Keycode const key)
{
    kb_on_keyup(self->keyboard, key);
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
            self->cycles_until_interrupt += cycles_per_frame;
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
