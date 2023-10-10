#include "SDL.h"
#include "display.h"
#include "spectrum.h"
#include "tap-loader.h"
#include "z80/z80.h"
#include <stdio.h>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;

struct Spectrum spectrum;

static void blit(void)
{
    uint32_t* pixels;
    int pitch;

    SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
    spec_render_display(&spectrum, pixels);
    SDL_UnlockTexture(texture);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

static void loop(void)
{
    int quit = 0;
    uint64_t time = SDL_GetPerformanceCounter();

    while (!quit)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_KEYDOWN: {
                    if (e.key.keysym.sym == SDLK_ESCAPE)
                    {
                        quit = 1;
                    }
                    else
                    {
                        spec_on_keydown(&spectrum, e.key.keysym.sym);
                    }

                    break;
                }

                case SDL_KEYUP: {
                    spec_on_keyup(&spectrum, e.key.keysym.sym);
                    break;
                }

                case SDL_QUIT: quit = 1; break;
            }
        }

        blit();

        uint64_t const now = SDL_GetPerformanceCounter();
        float const delta = (now - time) / (float)SDL_GetPerformanceFrequency();
        spec_run(&spectrum, spec_cycles(delta));
        time = now;
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

    spec_destruct(&spectrum);

    SDL_Quit();
}

int main(int argc, char** argv)
{
    atexit(cleanup);

    spec_construct(&spectrum);
    spec_load_rom(&spectrum, "./roms/48.rom");

    // @TODO make LOAD a method on spectrum
    if (!tap_construct(spectrum.tap, "./images/MANIC.TAP"))
    {
        puts("Could not create TAP image");
        exit(EXIT_FAILURE);
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        puts("cannot init video\n");
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("ZX Spectrum",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              display_width * 2,
                              display_height * 2,
                              0);

    renderer = SDL_CreateRenderer(window,
                                  -1,
                                  SDL_RENDERER_ACCELERATED
                                      | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
        fail("Could not create renderer");

    // @TODO can I query for render or window size?
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_RGBA32,
                                SDL_TEXTUREACCESS_STREAMING,
                                display_width,
                                display_height);

    loop();

    return 0;
}
