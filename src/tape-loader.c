#include "tape-loader.h"
#include "memory.h"
#include "tape-formats/tap-format.h"
#include "tape-formats/tzx-format.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** True if the file ends with the given extension, using a case insensitive
 * comparison.
 */
static bool has_extension(char const *filename, char const *ext)
{
    char const *dot = strrchr(filename, '.');
    if (!dot)
        return false;

    ++dot;
    while (*dot && *ext)
    {
        if (tolower(*dot) != tolower(*ext))
            return false;
        ++dot;
        ++ext;
    }

    return !*dot && !*ext;
}

struct Tape *tape_make(char const *filename)
{
    if (has_extension(filename, "tap"))
    {
        return tap_make(filename);
    }
    else if (has_extension(filename, "tzx"))
    {
        return tzx_make(filename);
    }
    else
    {
        return NULL;
    }
}

void tape_destroy(struct Tape *tape)
{
    (tape->destroy)(tape);
}

bool tape_load_next_block(struct Tape *tape,
                          uint8_t block_type,
                          uint16_t addr,
                          uint16_t length,
                          struct Memory *memory)
{
    struct TapeBlock block = (tape->get_next_block)(tape);
    if (!block.valid)
        return false;

    mem_write_block(memory, addr, block.data, block.length);

    return true;
}

struct TapeBlock invalid_block()
{
    struct TapeBlock tb;
    tb.valid = false;
    return tb;
}

struct TapeBlock valid_block(uint8_t block_type, uint16_t length, uint8_t const *data)
{
    struct TapeBlock tb;
    tb.valid = true;
    tb.block_type = block_type;
    tb.length = length;
    tb.data = data;
    return tb;
}
