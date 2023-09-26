#pragma once

#include "character.h"
#include "hashmap.h"
#include "level.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Minimal number of pixels to keep between the player and the screen border.
#define CHARACTER_SCROLL_BORDER_HORIZONTAL (WINDOW_WIDTH / 4.0)
#define CHARACTER_SCROLL_BORDER_VERTICAL (WINDOW_HEIGHT / 3.0)

#define WINDOW_NAME "Game"

#define BACKGROUND_COLOR 0x00, 0x00, 0x00, 0xFF

#define FPS 60

#define FRAME_DURATION (1000 / FPS)

#define GRAVITY 0.9

#define SCALING_FACTOR 2

#define TILE_SIZE 16

// NOTE: should not be larger than the smallest solid tile size.
// Otherwise the character will be able to clip trough it.
#define MAX_ACCELERATION (TILE_SIZE)

#define CHARACTER_SPEED 3
#define CHARACTER_JUMP_STRENGTH 30

typedef HASHMAP(char, Level) LevelHashmap;

/**
 * @brief Loads the levels stored in the given file paths using the given tileset.
 *
 * @param level_paths Paths to files with the level data.
 * @see level_load
 * @param size The size of the level_paths array.
 * @param tileset The tileset to use.
 * @return Dynamically allocated hashmap from level name to the level itself.
 *
 * @see unload_levels
 */
LevelHashmap *load_levels(const char **level_paths, size_t size,
                          Tileset *tileset);

/**
 * @brief Frees and cleans up the levels hashmap, and all the levels it stores.
 *
 * @param levels The levels hashmap to unload.
 * @see level_destroy
 */
void unload_levels(LevelHashmap *levels);

/**
 * @brief Calculates the offset for rendering so that the character stays on
 *          screen even if it's x/y pos is out of window's borders.
 *
 * @param character The character for which to calculate the offset.
 * @param previous_offset The previous offset for the renderer.
 * @param new_offset[out] The offset for the renderer.
 *
 * @see renderer_render_copy_with_offset_f
 */
void calculate_rendering_offset(const Character *character,
                                SDL_FPoint previous_offset,
                                SDL_FPoint *new_offset);
