#include "SDL.h"
#include "SDL_image.h"
#include "character.h"
#include "level.h"
#include "main.h"
#include "renderer.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        die("Usage: %s <Character Texture Path> <Level Path> <Tileset "
            "Path> <Textures path>",
            argv[0]);
    }

    const char *characterTexturePath = argv[1];
    const char *levelPath = argv[2];
    const char *tilesetPath = argv[3];
    const char *texturesDirPath = argv[4];

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (initSDL(&window, &renderer, WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT))
        die("Init SDL failed");

    FILE *tilesetFile = fopen(tilesetPath, "rb");

    if (!tilesetFile)
        die("Opening file %s failed", tilesetPath);

    // tileset_load also could fail, so by this we check both
    Tileset *tileset =
        tileset_load(tilesetFile, strdup(texturesDirPath), renderer);

    fclose(tilesetFile);

    if (!tileset)
        die("Loading tileset %s failed", tilesetPath);

    SDL_Texture *characterTexture =
        IMG_LoadTexture(renderer, characterTexturePath);
    if (!characterTexture)
        die("Loading %s failed", characterTexturePath);

    int w, h;
    SDL_QueryTexture(characterTexture, NULL, NULL, &w, &h);

    Character *character =
        character_create(characterTexture, (SDL_FRect){0, 0, w, h}, SCALING_FACTOR);

    FILE *levelFile = fopen(levelPath, "rb");
    if (!levelFile)
        die("Opening %s failed", levelPath);

    Level *level = level_load(levelFile, tileset, TILE_SIZE, TILE_SIZE, SCALING_FACTOR);
    if (!level)
        die("Loading level %s failed", levelPath);
    fclose(levelFile);

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
                tileset_destroy(tileset);
                SDL_DestroyTexture(characterTexture);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return EXIT_SUCCESS;
            }
        }

        character_applyGravity(character, GRAVITY);
        character_tick(character, level->layers[0]->tiles, MAX_ACCELERATION);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        level_draw(level, renderer);

        character_draw(character, renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(FRAME_DURATION);
    }

    return EXIT_SUCCESS;
}
