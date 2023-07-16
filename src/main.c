#include "SDL.h"
#include "SDL_image.h"
#include "character.h"
#include "level.h"
#include "main.h"
#include "renderer.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <Character Texture Path> <Tile Path>\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    const char *texture_path = argv[1];
    const char *tile_path = argv[2];

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (initSDL(&window, &renderer, WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "initSDL()",
                                 SDL_GetError(), 0);
        SDL_DestroyRenderer(renderer);
        SDL_Log("initSDL(): %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Texture *character_texture = IMG_LoadTexture(renderer, texture_path);
    SDL_Texture *tile_texture = IMG_LoadTexture(renderer, tile_path);
    if (!character_texture || !tile_texture)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Log("IMG_LoadTexture(): %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    int w, h;
    SDL_QueryTexture(character_texture, NULL, NULL, &w, &h);

    Character *character =
        character_create(character_texture, (SDL_FRect){0, 0, w * 10, h * 10});
    Level *level = level_create();

    SDL_QueryTexture(tile_texture, NULL, NULL, &w, &h);
    level_add_tile(level, (Tile){.hitbox = {0, 500, w * 10, h * 10},
                                 .solid = true,
                                 .texture = tile_texture});

    while (1)
    {
        SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR);
        SDL_RenderClear(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                level_destroy(level);
                character_destroy(character);
                SDL_DestroyTexture(character_texture);
                SDL_DestroyTexture(tile_texture);
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                return EXIT_SUCCESS;
            }
        }

        character_applyGravity(character, GRAVITY);
        character_tick(character, level->tiles, MAX_ACCELERATION);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        level_draw(level, renderer);

        character_draw(character, renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(FRAME_DURATION);
    }

    return EXIT_SUCCESS;
}
