#include "memory.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 0x4000

void mem_construct(struct Memory* self, bool is128)
{
    memset(self, 0, sizeof(*self));

    if (is128)
    {
        self->rom_banks[0] = calloc(1, PAGE_SIZE);
        self->rom_banks[1] = calloc(1, PAGE_SIZE);

        for (int i = 0; i < 8; ++i)
            self->ram_banks[i] = calloc(1, PAGE_SIZE);
    }
    else
    {
        self->rom_banks[0] = calloc(1, PAGE_SIZE);

        self->ram_banks[0] = calloc(1, PAGE_SIZE);
        self->ram_banks[2] = calloc(1, PAGE_SIZE);
        self->ram_banks[5] = calloc(1, PAGE_SIZE);
    }

    self->map[0] = self->rom_banks[0];
    self->map[1] = self->ram_banks[5];
    self->map[2] = self->ram_banks[2];
    self->map[3] = self->ram_banks[0];
    self->screen = self->ram_banks[5];
}

void mem_destruct(struct Memory* self)
{
    free(self->rom_banks[0]);
    free(self->rom_banks[1]);
    for (int i = 0; i < 8; ++i)
        free(self->ram_banks[i]);
}

void mem_set_rom(struct Memory* self, int const page, uint8_t const* data, uint16_t const length)
{
    assert(length <= PAGE_SIZE);
    assert(self->rom_banks[page]);
    memcpy(self->rom_banks[page], data, length);
}

void mem_write(struct Memory* self, uint16_t addr, uint8_t val)
{
    int const sel = addr >> 14;
    if (sel > 0)
        self->map[sel][addr & 0x3fff] = val;
}

void mem_write_block(struct Memory* self, uint16_t addr, uint8_t const* data, uint16_t length)
{
    for (int i = 0; i < length; ++i, ++addr, ++data)
        mem_write(self, addr, *data);
}

uint8_t mem_read(struct Memory* self, uint16_t addr)
{
    int const sel = addr >> 14;
    return self->map[sel][addr & 0x3fff];
}

void mem_page(struct Memory* self, uint8_t const val)
{
    self->map[3] = self->ram_banks[val & 0x07];
    self->map[0] = self->rom_banks[(val & 0x10) ? 1 : 0];
    //self->screen = self->ram_banks[(val & 0x08) ? 5 : 7];
}

uint8_t const* mem_screen(struct Memory* self)
{
    return self->screen;
}

void mem_reset(struct Memory* self)
{
    // @TODO impl this
}

