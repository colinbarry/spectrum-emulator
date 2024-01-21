#include "tzx-format.h"
#include "tape-loader.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct TzxBlock
{
    uint8_t* begin;
    uint8_t* end;
};

struct Tzx
{
    struct Tape tape;
    uint8_t* data;
    uint8_t const* end;
    uint8_t const* current;
};

static uint8_t readb(uint8_t const* data)
{
    return *data;
}

static uint16_t readw(uint8_t const* data)
{
    return *data | *(data + 1) << 8;
}

static bool is_info_block(uint8_t const id)
{
    return id == 0x32;
}

static struct TapeBlock get_next_block(struct Tape* tape)
{
    struct Tzx* tzx = (struct Tzx*)tape;
    uint8_t const* block = tzx->current;
    uint8_t const* const offset = block;

    uint8_t block_id = readb(block);
    ++block;

    while (is_info_block(block_id))
    {
        uint16_t const block_length = readw(block);
        block += 2 + block_length;
        block_id = readb(block);
        ++block;
    }

    uint16_t block_length;
    struct TapeBlock tb;
    if (block_id == 0x10)
    {
        block += 2; // skip PAUSE
        block_length = readw(block);
        block += 2;
        tb = valid_block(block_id, block_length - 2, block + 1);
    }
    else
    {
        // skip unknown block type
        block_length = readw(block);
        block += 2;
        tb = invalid_block();
    }

    tzx->current = block + block_length;
    if (tzx->current >= tzx->end)
    {
        tzx->current = tzx->data;
    }

    return tb;
}

static void destroy(struct Tape* tape)
{
    struct Tzx* tzx = (struct Tzx*)(tape);
    free(tzx->data);
    free(tzx);
}

static bool validate(uint8_t const* data, size_t const len)
{
    static const uint8_t expected_header[] = {'Z', 'X', 'T', 'a', 'p', 'e', '!', 0x1a};

    if (len < 10)
        return false;

    if (memcmp(data, expected_header, sizeof(expected_header)))
        return false;

    return true;
}

struct Tape* tzx_make(char const* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("cannot load \"%s\"\n", filename);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    int const len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    struct Tzx* tzx = malloc(sizeof(*tzx));
    tzx->data = malloc(len);
    tzx->current = tzx->data + 10;
    tzx->end = tzx->current + len;

    fread(tzx->data, 1, len, fp);
    fclose(fp);

    if (!validate(tzx->data, len))
    {
        destroy(&tzx->tape);
        return NULL;
    }

    tzx->tape.get_next_block = get_next_block;
    tzx->tape.destroy = destroy;

    return &tzx->tape;
}
