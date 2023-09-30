#include <stdio.h>
#include "z80/z80.h"
#include "spectrum.h"
#include "display.h"
#include "SDL.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

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
    int i;

    while (!quit)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_KEYDOWN:
                {
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

                case SDL_KEYUP:
                {
                   spec_on_keyup(&spectrum, e.key.keysym.sym);
                   break;
                }

                case SDL_QUIT:
                    quit = 1;
                    break;
            }
        }

        spec_frame(&spectrum);
        blit();
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

int main(int argc, char **argv)
{
    spec_construct(&spectrum);
    spec_load_rom(&spectrum, "./roms/48.rom");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        puts("cannot init video\n");
        exit(EXIT_FAILURE);
    }

    atexit(cleanup);

    window = SDL_CreateWindow("ZX Spectrum", 
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            display_width * 2,
            display_height * 2,
            0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
        fail("Could not create renderer");

    // @TODO can I query for render or window size?
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, display_width, display_height);

    loop();

    return 0;
}
