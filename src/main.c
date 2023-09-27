#include "SDL.h"
#include "SDL_image.h"
#include "character.h"
#include "hashmap.h"
#include "level.h"
#include "main.h"
#include "renderer.h"
#include "tile_keyboard_events.h"
#include "utils.h"
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

    const char *character_texture_path = argv[1];
    const char *tileset_path = argv[2];
    const char *textures_dir_path = argv[3];
    const char *keymap_path = argv[4];
    const char *level_name = argv[5];
    const char **level_paths = (const char **)(argv + 6);
    int level_amount = argc - 6;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    init_sdl(&window, &renderer);

    FILE *tileset_file = fopen(tileset_path, "rb");

    if (!tileset_file)
        die("Opening file %s failed", tileset_path);

    Tileset *tileset =
        tileset_load(tileset_file, strdup(textures_dir_path), renderer);

    fclose(tileset_file);

    if (!tileset)
        die("Loading tileset %s failed", tileset_path);

    FILE *keymap_file = fopen(keymap_path, "rb");

    if (!keymap_file)
        die("Opening file %s failed", keymap_path);

    KeyEventSubscribers *event_subscribers =
        tile_keyboard_mappings_load(keymap_file, tileset);

    fclose(keymap_file);

    if (!event_subscribers)
        die("Loading keymap %s failed", keymap_path);

    SDL_Texture *character_texture =
        IMG_LoadTexture(renderer, character_texture_path);
    if (!character_texture)
        die("Loading %s failed", character_texture_path);

    int w, h;
    SDL_QueryTexture(character_texture, NULL, NULL, &w, &h);

    Character *character = character_create(
        character_texture, (SDL_FRect){0, 0, w, h}, CHARACTER_SPEED,
        CHARACTER_JUMP_STRENGTH, SCALING_FACTOR);

    LevelHashmap *levels = levels_load(level_paths, level_amount, tileset, TILE_SIZE, TILE_SIZE, SCALING_FACTOR);

    Level *current_level = hashmap_get(levels, level_name);
    if (!current_level)
        die("Level %s not found", level_name);

    SDL_FPoint rendering_offset = {0};

    CallbackGameState callback_game_state = {
        .level_ptr = &current_level,
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
                    tile_keyboard_events_notify(event_subscribers,
                                                event.key.keysym.sym,
                                                &callback_game_state);
                    /* fallthrough */
                case SDL_KEYUP:
                    character_handle_keyboard_event(character, &event.key);
            }
        }

        character_apply_gravity(character, GRAVITY);
        character_tick(character, current_level->layers, MAX_ACCELERATION);

        calculate_rendering_offset(character, rendering_offset,
                                   &rendering_offset);

        level_draw(current_level, renderer, &rendering_offset);

        character_draw(character, renderer, &rendering_offset);
        SDL_RenderPresent(renderer);

        SDL_Delay(FRAME_DURATION);
    }

    tile_keyboard_events_destroy(event_subscribers);
    levels_unload(levels);
    character_destroy(character);
    tileset_destroy(tileset);
    SDL_DestroyTexture(character_texture);
    quit_sdl(window, renderer);

    return EXIT_SUCCESS;
}

void calculate_rendering_offset(const Character *character,
                                const SDL_FPoint previous_offset,
                                SDL_FPoint *new_offset)
{
    const SDL_FPoint character_pos = character_get_position(character);

    const SDL_FPoint on_screen_pos = {character_pos.x + previous_offset.x,
                                      character_pos.y + previous_offset.y};

    float left_boundary = CHARACTER_SCROLL_BORDER_HORIZONTAL;
    float right_boundary = WINDOW_WIDTH - CHARACTER_SCROLL_BORDER_HORIZONTAL;

    new_offset->x =
        -(character_pos.x -
          SDL_clamp(on_screen_pos.x, left_boundary, right_boundary));

    left_boundary = CHARACTER_SCROLL_BORDER_VERTICAL;
    right_boundary = WINDOW_HEIGHT - CHARACTER_SCROLL_BORDER_VERTICAL;

    new_offset->y =
        -(character_pos.y -
          SDL_clamp(on_screen_pos.y, left_boundary, right_boundary));
}

void init_sdl(SDL_Window **window_ptr, SDL_Renderer **renderer_ptr)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        die("SDL_Init: %s", SDL_GetError());

    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, window_ptr,
                                    renderer_ptr) < 0)
        die("SDL_CreateWindowAndRenderer: %s", SDL_GetError());

    SDL_SetWindowTitle(*window_ptr, WINDOW_NAME);
    SDL_SetWindowPosition(*window_ptr, SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);
    if (!SDL_RenderSetVSync(*renderer_ptr, true))
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "SDL_RenderSetVSync: %s",
                    SDL_GetError());
}

void quit_sdl(SDL_Window *window, SDL_Renderer *renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
