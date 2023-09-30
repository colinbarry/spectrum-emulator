#ifndef SPECTRUM_KEYBOARD_H
#define SPECTRUM_KEYBOARD_H

#include <stdint.h>
#include "SDL.h"

struct Keyboard {
    int keys[40];
};

void kb_construct(struct Keyboard* self);

uint8_t kb_read(struct Keyboard const* self, uint16_t port);

void kb_on_keydown(struct Keyboard* self, SDL_Keycode key);
void kb_on_keyup(struct Keyboard* self, SDL_Keycode key);

#endif // SPECTRUM_KEYBOARD_H
