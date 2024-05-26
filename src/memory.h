#ifndef SPECTRUM_MEMORY_H_
#define SPECTRUM_MEMORY_H_

#include <stdbool.h>
#include <stdint.h>

#define MEMORY_CAPACITY 65536

struct Memory
{
    uint8_t *rom_banks[2];
    uint8_t *ram_banks[8];
    bool disable_switch;
    uint8_t *map[4];
    uint8_t *screen;
};

void mem_construct(struct Memory *self, bool is128);
void mem_destruct(struct Memory *self);

void mem_set_rom(struct Memory *self, int page, uint8_t const *data, uint16_t length);

void mem_write(struct Memory *self, uint16_t addr, uint8_t val);
void mem_write_block(struct Memory *self,
                     uint16_t addr,
                     uint8_t const *data,
                     uint16_t length);
uint8_t mem_read(struct Memory *self, uint16_t addr);

void mem_reset(struct Memory *self);

void mem_page(struct Memory *self, uint8_t val);
uint8_t const *mem_screen(struct Memory *self);

#endif // SPECTRUM_MEMORY_H_
