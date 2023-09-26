#include "SDL.h"
#include "SDL_image.h"
#include "character.h"
#include "hashmap.h"
#include "level.h"
#include "main.h"
#include "renderer.h"
#include "tile_keyboard_events.h"
#include "utils.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        die("Usage: %s <Character Texture Path> <Tileset Path> <Textures path> "
            "<KeyMap path>"
            "<Starting level name> [Level paths...]",
            argv[0]);
    }

    const char *characterTexturePath = argv[1];
    const char *tilesetPath = argv[2];
    const char *texturesDirPath = argv[3];
    const char *keymapPath = argv[4];
    const char *levelName = argv[5];
    const char **levelPaths = (const char **)(argv + 6);
    int levelAmount = argc - 6;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (initSDL(&window, &renderer, WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT))
        die("Init SDL failed");

    FILE *tilesetFile = fopen(tilesetPath, "rb");

    if (!tilesetFile)
        die("Opening file %s failed", tilesetPath);

    Tileset *tileset =
        tileset_load(tilesetFile, strdup(texturesDirPath), renderer);

    fclose(tilesetFile);

    if (!tileset)
        die("Loading tileset %s failed", tilesetPath);

    FILE *keymapFile = fopen(keymapPath, "rb");

    if (!keymapFile)
        die("Opening file %s failed", keymapPath);

    KeyEventSubscribers *eventSubscribers =
        tile_keyboard_mappings_load(keymapFile, tileset);

    fclose(keymapFile);

    if (!eventSubscribers)
        die("Loading keymap %s failed", keymapPath);

    SDL_Texture *characterTexture =
        IMG_LoadTexture(renderer, characterTexturePath);
    if (!characterTexture)
        die("Loading %s failed", characterTexturePath);

    int w, h;
    SDL_QueryTexture(characterTexture, NULL, NULL, &w, &h);

    Character *character = character_create(
        characterTexture, (SDL_FRect){0, 0, w, h}, CHARACTER_SPEED,
        CHARACTER_JUMP_STRENGTH, SCALING_FACTOR);

    LevelHashmap *levels = loadLevels(levelPaths, levelAmount, tileset);

    Level *currentLevel = hashmap_get(levels, levelName);
    if (!currentLevel)
        die("Level %s not found", levelName);

    SDL_FPoint renderingOffset = {0};

    CallbackGameState callback_game_state = {
        .level_ptr = &currentLevel,
        .character = character,
    };

    bool done = false;
    while (!done)
    {
        SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR);
        SDL_RenderClear(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_KEYDOWN:
                    tile_keyboard_events_notify(eventSubscribers,
                                                event.key.keysym.sym,
                                                &callback_game_state);
                    /* fallthrough */
                case SDL_KEYUP:
                    character_handleKeyboardEvent(character, &event.key);
            }
        }

        character_applyGravity(character, GRAVITY);
        character_tick(character, currentLevel->layers, MAX_ACCELERATION);

        calculateRenderingOffset(character, renderingOffset, &renderingOffset);

        level_draw(currentLevel, renderer, &renderingOffset);

        character_draw(character, renderer, &renderingOffset);
        SDL_RenderPresent(renderer);

        SDL_Delay(FRAME_DURATION);
    }

    tile_keyboard_events_destroy(eventSubscribers);
    unloadLevels(levels);
    character_destroy(character);
    tileset_destroy(tileset);
    SDL_DestroyTexture(characterTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void calculateRenderingOffset(const Character *character,
                              const SDL_FPoint previousOffset,
                              SDL_FPoint *newOffset)
{
    const SDL_FPoint characterPos = character_getPosition(character);

    const SDL_FPoint onScreenPos = {characterPos.x + previousOffset.x,
                                    characterPos.y + previousOffset.y};

    float leftBoundary = CHARACTER_SCROLL_BORDER_HORIZONTAL;
    float rightBoundary = WINDOW_WIDTH - CHARACTER_SCROLL_BORDER_HORIZONTAL;

    newOffset->x = -(characterPos.x -
                     SDL_clamp(onScreenPos.x, leftBoundary, rightBoundary));

    leftBoundary = CHARACTER_SCROLL_BORDER_VERTICAL;
    rightBoundary = WINDOW_HEIGHT - CHARACTER_SCROLL_BORDER_VERTICAL;

    newOffset->y = -(characterPos.y -
                     SDL_clamp(onScreenPos.y, leftBoundary, rightBoundary));
}

LevelHashmap *loadLevels(const char **levelPaths, size_t size, Tileset *tileset)
{
    LevelHashmap *levels = malloc(sizeof(*levels));
    hashmap_init(levels, hashmap_hash_string, strcmp);

    for (size_t i = 0; i < size; i++)
    {
        FILE *file = fopen(levelPaths[i], "rb");

        if (!file)
            die("Opening file %s failed", levelPaths[i]);

        Level *level =
            level_load(file, tileset, TILE_SIZE, TILE_SIZE, SCALING_FACTOR);

        fclose(file);

        int err = hashmap_put(levels, level->name, level);

        if (err == -EEXIST)
            die("Names of levels should be unique, but %s appeared more than "
                "once",
                level->name);
        else if (err)
            die("Error while loading level %s - %s", level, strerror(-err));
    }

    return levels;
}

void unloadLevels(LevelHashmap *levels)
{
    const char *key;
    void *temp;

    // The reason for the warning "Missing field 'iter_types' initializer" is a
    // 0 width array "not" being initialized.
    hashmap_foreach_key_safe(key, levels, temp)
    {
        Level *level = hashmap_remove(levels, key);
        level_destroy(level);
    }

    hashmap_cleanup(levels);
    free(levels);
}
