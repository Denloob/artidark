#include "SDL.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

int initSDL(SDL_Window **window, SDL_Renderer **renderer,
            const char *windowName, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return EXIT_FAILURE;

    if (initWindow(window, windowName, width, height) ||
        initRenderer(renderer, window))
    {
        if (*window)
            SDL_DestroyWindow(*window);
        SDL_Quit();

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int initWindow(SDL_Window **window, const char *windowName, int width,
               int height)
{
    *window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, width, height, 0);

    return *window ? EXIT_SUCCESS : EXIT_FAILURE;
}

int initRenderer(SDL_Renderer **renderer, SDL_Window **window)
{
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_PRESENTVSYNC);

    return *renderer ? EXIT_SUCCESS : EXIT_FAILURE;
}
