#ifndef SPECTRUM_MEMORY_H_
#define SPECTRUM_MEMORY_H_

#include <stdbool.h>
#include <stdint.h>

#define MEMORY_CAPACITY 65536

/** State of the Spectrum's memory.
 */
struct Memory
{
    uint8_t *rom_banks[2];
    uint8_t *ram_banks[8];
    bool disable_switch;
    uint8_t *map[4];
    uint8_t *screen;
};

/** Constructs a Memory object.
 * @param self Pointer to the memory object.
 * @param is128 When true, the memory will be initialized with 128K support.
 */
void mem_construct(struct Memory *self, bool is128);

/** Destructs a previously constructed Memory object.
 */
void mem_destruct(struct Memory *self);

/* Copies a ROM file into the given page of the memory.
 * @param self The memory
 * @param page Index of the destination page
 * @param data Pointer to ROM file data
 * @param length Length of ROM file data (in bytes)
 */
void mem_set_rom(struct Memory *self, int page, uint8_t const *data, uint16_t length);

/** Writes a single 8-bit value to a memory location.
 * @param self The memory
 * @param addr The 16-bit address of the write. The actual destination will
 * depend upon the previously set page.
 * @param val This 8-bit value will be written to the memory
 */
void mem_write(struct Memory *self, uint16_t addr, uint8_t val);

/** Writes a contiguous block of 8-bit values to the memory
 * @param self The memory
 * @param addr Destination address of the write
 * @param data Pointer to the source data
 * @param length Length of source data, in bytes.
 */
void mem_write_block(struct Memory *self,
                     uint16_t addr,
                     uint8_t const *data,
                     uint16_t length);

/** Reads from the given memory address.
 */
uint8_t mem_read(struct Memory *self, uint16_t addr);

/** Resets the memory.
 */
void mem_reset(struct Memory *self);

/** Configures the memory so that certain pages are activated.
 */
void mem_page(struct Memory *self, uint8_t val);

/** Returns the current page used for the video memory.
 */
uint8_t const *mem_screen(struct Memory *self);

#endif // SPECTRUM_MEMORY_H_
