#ifndef SPECTRUM_SPECTRUM_H_
#define SPECTRUM_SPECTRUM_H_

#include "SDL.h"
#include <stdbool.h>
#include <stdint.h>

struct Keyboard;
struct Z80;
struct Tape;
struct Memory;

/** Supported Spectrum models
 */
enum Model
{
    s48, /**< The 48K model */
    s128 /**< The 128K model  */
};

/** State of the entire Spectrum emulation.
 */
struct Spectrum
{
    enum Model model;
    struct Memory *memory;
    struct Keyboard *keyboard;
    struct Z80 *z80;
    struct Tape *tape;
    int frame;
    int cycles_until_interrupt;
    int8_t border_attr;
};

/** Initializes a Spectrum of the given model.
 * @param self Pointer to the Spectrum state.
 * @param Model The model of Spectrum,
 */
void spec_construct(struct Spectrum *self, enum Model model);

/** Destructs a previously initialized Spectrum.
 * @param self Pointer to previously initialized Spectrum.
 */
void spec_destruct(struct Spectrum *self);

/** Loads a ROM into the given page.
 * @param self The Spectrum
 * @param page Index of the page in which to load the ROM.
 * @param filename Path to ROM file on disk.
 */
void spec_load_rom(struct Spectrum *self, int page, char const *filename);

/** Mounts a tape file (in any format) onto the Spectrum.
 * @param self The Spectrum
 * @param filename Path to the tape file.
 */
bool spec_insert_tape(struct Spectrum *self, char const *filename);

/** Notify the Spectrum that an SDL key has been pressed. @TODO move these
 * functions as the Spectrum need not know about SDL.
 */
void spec_on_keydown(struct Spectrum *self, SDL_Keycode key);
/** Notify the Spectrum that an SDL key has been released.
 */
void spec_on_keyup(struct Spectrum *self, SDL_Keycode key);

/** Runs the Spectrum simulator for a number of frames.
 * @param self The Spectrum
 * @param cycles The number of cycles to run the simulation.
 */
void spec_run(struct Spectrum *self, int cycles);

/** Renders the current state of the Spectrum's video memory to the given
 * surface.
 * @param self The Spectrum.
 * @param surface Pointer to the surface to which the Spectrum will render. This
 * is a buffer in RGBA format.
 */
void spec_render_display(struct Spectrum *self, uint32_t *surface);

/** The number of cycles that will be processed in the given number of seconds
 */
int spec_cycles(float seconds);

#endif // SPECTRUM_SPECTRUM_H_
