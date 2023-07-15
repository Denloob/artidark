#include "SDL.h"
#include "character.h"
#include "level.h"
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

    Character character = {.hitbox = {0, 0, 100, 100}, .velocity = {0, 0}};
    Level *level = level_create();
    level_add_tile(level, (Tile){.hitbox = {0, 500, 50, WINDOW_HEIGHT}, .solid=true});

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

        character_applyGravity(&character, GRAVITY);
        character_tick(&character, level->tiles);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        character_draw(&character, renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(FRAME_DURATION);
    }

    return EXIT_SUCCESS;
}
