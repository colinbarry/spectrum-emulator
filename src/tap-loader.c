#include "tap-loader.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint16_t readw(uint8_t const* data)
{
    return *data | *(data + 1) << 8;
}

static void trace_blocks(uint8_t const* data, size_t const length)
{
    uint8_t const* b = data;
    uint8_t const* e = data + length;

    while (b != e)
    {
        uint16_t const block_data_length = readw(b) - 2;
        b += 2;

        printf("BLOCK LEN=0x%04x ", block_data_length);

        uint8_t const f = *b++;
        if (f == 0x00)
        {
            printf("HEADER ");

            uint8_t const type = *b++;
            printf("TYPE:%i ", type);

            for (int i = 0; i < 10; ++i)
            {
                printf("%c", *b++);
            }

            uint16_t const db_len = readw(b);
            b += 2;

            uint16_t const p1 = readw(b);
            b += 2;

            uint16_t const p2 = readw(b);
            b += 2;

            printf("DBLEN:0x%04x P1:0x%04x P2:0x%04x ", db_len, p1, p2);
        }
        else
        {
            printf("DATA ");
            b += block_data_length;
        }
        printf(" CHKSUM: 0x%02x\n", *b++);
    }
}

bool tap_construct(struct Tap* tap, char const* filename)
{
    memset(tap, 0, sizeof(*tap));

    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("cannot load \"%s\"\n", filename);
        return false;
    }

    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    tap->data = malloc(len);
    tap->current = tap->data;
    tap->end = tap->data + len;

    fread(tap->data, 1, len, fp);
    fclose(fp);

    trace_blocks(tap->data, len);

    return true;
}

void tap_destruct(struct Tap* tap)
{
    free(tap->data);
}

int tap_load_next_block(struct Tap* tap,
                        uint8_t type,
                        uint16_t addr,
                        uint16_t data_length,
                        struct Memory* memory)
{
    uint8_t const* block = tap->current;
    uint16_t block_length = readw(block);
    block += 2;

    tap->current += block_length + 2;
    if (tap->current >= tap->end)
    {
        tap->current = tap->data;
    }

    uint8_t const block_type = *block;
    if (block_type != type || block_length != data_length + 2)
    {
        printf("LEN ERR: Expected 0x%04x, got 0x%04x", data_length, block_length);
        exit(0);
        return 0;
    }

    uint8_t computed_chk = 0;
    for (int i = 0; i < block_length - 1; ++i)
        computed_chk ^= block[i];

    uint8_t const expected_chk = block[block_length - 1];
    if (computed_chk != expected_chk)
    {
        printf("CHKSUM ERR: Expected 0x%02x, got 0x%02x", expected_chk, computed_chk);
        return 1;
    }

    mem_write_block(memory, addr, block + 1, data_length);

    printf("LOADED BLOCK: 0x%04x <- 0x%04x\n", addr, data_length);

    return 1;
}
