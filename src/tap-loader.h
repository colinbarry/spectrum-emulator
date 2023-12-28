#ifndef SPECTRUM_TAP_LOADER_H
#define SPECTRUM_TAP_LOADER_H

#include <stdbool.h>
#include <stdlib.h>

struct Memory;

struct Tape
{
    int (*load_next_block)(struct Tape* self,
                           uint8_t block_type,
                           uint16_t addr,
                           uint16_t length,
                           struct Memory* memory);
    void (*destroy)(struct Tape* self);
};

struct Tape* tape_make(char const* filename);

int tape_load_next_block(struct Tape* tape,
                         uint8_t block_type,
                         uint16_t addr,
                         uint16_t length,
                         struct Memory* memory);

void tape_destroy(struct Tape* tape);

#endif // SPECTRUM_TAP_LOADER_H
