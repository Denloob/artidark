#include "SDL.h"
#include "main.h"
#include "renderer.h"
#include <stdlib.h>

int main(void)
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (initSDL(&window, &renderer, WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "initSDL()",
                                 SDL_GetError(), 0);
        SDL_Log("initSDL(): %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    while (1)
    {
        SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR);
        SDL_RenderClear(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                return EXIT_SUCCESS;
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(FRAME_DURATION);
    }

    return EXIT_SUCCESS;
}
