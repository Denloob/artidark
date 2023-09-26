#include "SDL.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

int init_SDL(SDL_Window **window, SDL_Renderer **renderer,
             const char *window_name, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return EXIT_FAILURE;

    if (init_window(window, window_name, width, height) ||
        init_renderer(renderer, window))
    {
        if (*window)
            SDL_DestroyWindow(*window);
        SDL_Quit();

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int init_window(SDL_Window **window, const char *window_name, int width,
                int height)
{
    *window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, width, height, 0);

    return *window ? EXIT_SUCCESS : EXIT_FAILURE;
}

int init_renderer(SDL_Renderer **renderer, SDL_Window **window)
{
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_PRESENTVSYNC);

    return *renderer ? EXIT_SUCCESS : EXIT_FAILURE;
}

int renderer_render_copy_with_offset_f(SDL_Renderer *renderer,
                                       SDL_Texture *texture,
                                       const SDL_Rect *srcrect,
                                       const SDL_FRect *dstrect,
                                       SDL_FPoint *offset)
{
    const float x_offset = offset ? offset->x : 0;
    const float y_offset = offset ? offset->y : 0;
    const SDL_FRect dstrect_with_offset = {
        .x = dstrect->x + x_offset,
        .y = dstrect->y + y_offset,
        .w = dstrect->w,
        .h = dstrect->h,
    };

    return SDL_RenderCopyF(renderer, texture, srcrect, &dstrect_with_offset);
}
