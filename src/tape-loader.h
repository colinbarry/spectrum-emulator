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
    uint8_t const *data;
};

/** Constructs a TapeBlock referencing an invalid block.
 */
struct TapeBlock invalid_block();

/** Constructs a TapeBlock for the given data.
 * @param block_type
 * @param length The length of the block data (in bytes)
 * @param data
 */
struct TapeBlock valid_block(uint8_t block_type, uint16_t length, uint8_t const *data);

struct Tape
{
    struct TapeBlock (*get_next_block)(struct Tape *self);
    void (*destroy)(struct Tape *self);
};

/** Loads the given tape file and returns a Tape representing it.
 */
struct Tape *tape_make(char const *filename);

/** Loads the next block in the given Tape.
 * @param tape The tape
 * @param block-type The expected block type
 * @param addr The destination addr where the block will be loaded.
 * @param length The expected length of the block data.
 * @param memory The block will be loaded into this memory.
 * @return true if the block matched the expected type and could be loaded
 */
bool tape_load_next_block(struct Tape *tape,
                          uint8_t block_type,
                          uint16_t addr,
                          uint16_t length,
                          struct Memory *memory);

/** Destroys a previously initialized Tape object.
 */
void tape_destroy(struct Tape *tape);

#endif // SPECTRUM_TAPE_LOADER_H
