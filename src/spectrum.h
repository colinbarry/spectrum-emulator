#ifndef SPECTRUM_SPECTRUM_H_
#define SPECTRUM_SPECTRUM_H_

#include "SDL.h"
#include <stdbool.h>
#include <stdint.h>

struct Keyboard;
struct Z80;
struct Tape;
struct Memory;

enum Model
{
    s48,
    s128
};

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

void spec_construct(struct Spectrum *self, enum Model model);
void spec_destruct(struct Spectrum *self);

void spec_load_rom(struct Spectrum *self, int page, char const *filename);

bool spec_insert_tape(struct Spectrum *self, char const *filename);

void spec_on_keydown(struct Spectrum *self, SDL_Keycode key);
void spec_on_keyup(struct Spectrum *self, SDL_Keycode key);

void spec_run(struct Spectrum *self, int frames);
void spec_render_display(struct Spectrum *self, uint32_t *surface);

/** The number of cycles that will be processed in the given number of seconds
 */
int spec_cycles(float seconds);

#endif // SPECTRUM_SPECTRUM_H_
