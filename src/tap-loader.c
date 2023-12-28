#include "tap-loader.h"
#include "memory.h"
#include "tape-formats/tap-format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Tape* tape_make(char const* filename)
{
    return tap_make(filename);
}

void tape_destroy(struct Tape* tape)
{
    (tape->destroy)(tape);
}

int tape_load_next_block(struct Tape* tape,
                         uint8_t block_type,
                         uint16_t addr,
                         uint16_t length,
                         struct Memory* memory)
{
    return (tape->load_next_block)(tape, block_type, addr, length, memory);
}
