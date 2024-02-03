#ifndef SPECTRUM_TAPE_LOADER_H
#define SPECTRUM_TAPE_LOADER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct Memory;

struct TapeBlock
{
    bool valid;
    uint8_t block_type;
    uint16_t length;
    uint8_t const* data;
};

struct TapeBlock invalid_block();
struct TapeBlock valid_block(uint8_t block_type, uint16_t length, uint8_t const* data);

struct Tape
{
    struct TapeBlock (*get_next_block)(struct Tape* self);
    void (*destroy)(struct Tape* self);
};

struct Tape* tape_make(char const* filename);

bool tape_load_next_block(struct Tape* tape,
                          uint8_t block_type,
                          uint16_t addr,
                          uint16_t length,
                          struct Memory* memory);

void tape_destroy(struct Tape* tape);

#endif // SPECTRUM_TAPE_LOADER_H
