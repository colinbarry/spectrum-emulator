#include <stdio.h>
#include "z80/z80.h"
#include "SDL.h"

#define WIDTH (256 * 2)
#define HEIGHT (192 * 2)

uint8_t memory[65536] = {0};
struct Z80 z80;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

static uint8_t mem_load(struct Z80* z80, uint16_t const addr)
{
    return ((uint8_t*)z80->userdata)[addr];
}

static void mem_store(struct Z80* z80, uint16_t const addr, uint8_t const value)
{
    ((uint8_t*)z80->userdata)[addr] = value;
}

static uint8_t port_load(struct Z80* z80, uint8_t const port)
{
    printf("port load 0x%02x\n", port);
    return 0;
}

static void port_store(struct Z80* z80, uint8_t port, uint8_t const val)
{
    printf("port store 0x%02x = 0x%02x\n", port, val);
}

static uint32_t colour(uint8_t const attr)
{
    uint8_t const g = (attr & 0x04) ? 0xee : 0x00;
    uint8_t const r = (attr & 0x02) ? 0xee : 0x00;
    uint8_t const b = (attr & 0x01) ? 0xee : 0x00;
    return r | (g << 8) | (b << 16) | (0xff << 24);
}

static void blit(void)
{
    uint32_t* pixels;
    int pitch;

    SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

    for (int r = 0; r < 24; ++r) {
        for (int c = 0; c < 32; ++c) {
            uint32_t* wr = pixels + r * 2048 + c * 8;
            uint8_t const attr = memory[0x5800 + r * 0x20 + c];

            uint32_t const irgb = colour(attr);
            uint32_t const prgb = colour(attr >> 3);

            uint16_t addr;
            if (r < 8) {
                addr = 0x4000 + r * 0x20 + c;
            } else if (r < 16) {
                addr = 0x4800 + (r % 8) * 0x20 + c;
            } else {
                addr = 0x5000 + (r % 8) * 0x20 + c;
            }

            for (int i = 0; i < 8; ++i) {
               uint8_t byte = memory[addr];

                for (int j = 0; j < 8; ++j) {
                    *wr++ = (byte & 0x80) ? irgb : prgb;
                    byte <<= 1;
                }

                addr += 0x100;
                wr += 256 - 8;
            }
        }
    }

    SDL_UnlockTexture(texture);
}

static void loop(void)
{
    int quit = 0;
    int i;

    while (!quit)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_QUIT:
                    quit = 1;
                    break;
            }
        }

        for (i = 0; i < 10000; ++i) {
            z80_step(&z80);
        }

        blit();
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        SDL_RenderPresent(renderer);
    }
}

static void fail(char const* msg)
{
    printf("%s: %s\n", msg, SDL_GetError());
    exit(EXIT_FAILURE);
}

static void cleanup(void)
{
    if (texture)
        SDL_DestroyTexture(texture);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);

    SDL_Quit();
}

static void setupz80(void)
{
    FILE *romfile;
    int length;

    if ((romfile = fopen("./roms/48.rom", "rb")) == NULL) {
        printf("could not open rom file\n");
        exit(EXIT_FAILURE);
    }

    fseek(romfile, 0, SEEK_END);
    length = ftell(romfile);
    fseek(romfile, 0, SEEK_SET);
    printf("loading rom of size %i\n", length);

    if (length > sizeof(memory))
        length = sizeof(memory);
    fread(memory, 1, length, romfile);
    fclose(romfile);
    z80_init(&z80);
    z80.userdata = memory;
    z80.mem_load = &mem_load;
    z80.mem_store = &mem_store;
    z80.port_load = &port_load;
    z80.port_store = &port_store;

}

int main(int argc, char **argv)
{
    setupz80();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        puts("cannot init video\n");
        exit(EXIT_FAILURE);
    }

    atexit(cleanup);

    window = SDL_CreateWindow("ZX Spectrum", 
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WIDTH,
            HEIGHT,
            0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
        fail("Could not create renderer");

    // @TODO can I query for render or window size?
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 256, 192);

    loop();

    return 0;
}
