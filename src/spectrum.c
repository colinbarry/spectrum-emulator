#include "spectrum.h"
#include "display.h"
#include "keyboard.h"
#include <z80/z80.h>

static const int memory_capacity = 65536;
static const int64_t clock_speed = 3500000;
static const int frames_per_second = 50;
static const int cycles_per_frame = clock_speed / frames_per_second;

static uint8_t mem_load(struct Z80* z80, uint16_t const addr)
{
    struct Spectrum* spectrum = (struct Spectrum*)(z80->userdata);
    return (spectrum->memory)[addr];
}

static void mem_store(struct Z80* z80, uint16_t const addr, uint8_t const value)
{
    struct Spectrum* spectrum = (struct Spectrum*)(z80->userdata);
    (spectrum->memory)[addr] = value;
}

static uint8_t port_load(struct Z80* z80, uint16_t const port)
{
    struct Spectrum* spectrum = (struct Spectrum*)(z80->userdata);
    if (port == 0xfefe || port == 0xfdfe || port == 0xfbfe || port == 0xf7fe
        || port == 0xeffe || port == 0xdffe || port == 0xbffe || port == 0x7ffe)
    {
        return kb_read(spectrum->keyboard, port);
    }
    else
    {
        return 0xff;
    }
}

static void port_store(struct Z80* z80, uint16_t port, uint8_t const val)
{
    struct Spectrum* spectrum = (struct Spectrum*)(z80->userdata);
    if (port == 0xfe)
    {
        spectrum->border_attr = val & 0x07;
    }
}

//=============================================================================

void spec_construct(struct Spectrum* self)
{
    self->memory = calloc(memory_capacity, sizeof(uint8_t));
    self->keyboard = malloc(sizeof(*self->keyboard));
    self->z80 = malloc(sizeof(*self->z80));

    kb_construct(self->keyboard);

    self->cycles_until_interrupt = cycles_per_frame;

    z80_init(self->z80);
    self->z80->userdata = self;
    self->z80->mem_load = &mem_load;
    self->z80->mem_store = &mem_store;
    self->z80->port_load = &port_load;
    self->z80->port_store = &port_store;
}

void spec_destruct(struct Spectrum* self)
{
    free(self->z80);
    free(self->keyboard);
    free(self->memory);
}

void spec_load_rom(struct Spectrum* self, char const* filename)
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
    printf("loading rom of size %i\n", length);

    if (length > memory_capacity)
        length = memory_capacity;
    fread(self->memory, 1, length, romfile);
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
            // @TODO report cycles in handling interrupts
            z80_interrupt(self->z80, 0);
            self->cycles_until_interrupt += cycles_per_frame;
        }
    }
}

void spec_render_display(struct Spectrum* self, uint32_t* surface)
{
    display_render(surface, self->memory, self->border_attr, self->frame >= 16);
    self->frame = (self->frame + 1) % 32;
}

int spec_cycles(float const seconds)
{
    return (int)(seconds * clock_speed);
}
