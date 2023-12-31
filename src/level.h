#pragma once

#include "SDL.h"
#include "hashmap.h"
#include "level_layer.h"
#include "tileset.h"

#define LEVEL_LAYER_SEPARATOR '\\'

typedef struct Level
{
    char *name;
    VecLevelLayer layers;
} Level;

HASHMAP_NAMED_TYPEDEF(LevelHashmap, char, Level)

/**
 * @brief Creates a level.
 *
 * @param name The name of the level. Managed by the level.
 * @warning The name will be freed by the destructor.
 * @return The created level.
 */
Level *level_create(char *name);

/**
 * @brief Destroys the level.
 *
 * @param level The level to destroy.
 */
void level_destroy(Level *level);

/**
 * @brief Adds a layer to the level.
 *
 * @param layer The layer to add.
 */
void level_add_layer(Level *level, LevelLayer *layer);

/**
 * @brief Draws the level.
 *
 * @param renderer The renderer to draw onto.
 * @param offset The offset to apply to each of the layers.
 *
 * @see level_layer_draw
 */
void level_draw(const Level *level, SDL_Renderer *renderer, SDL_FPoint *offset);

/**
 * @brief Loads a level from a file.
 *
 * @param stream The stream to load level from. First line is expected to be
 *                  the name of the level.
 * @param tileset The tileset to use for the textures.
 * @param tile_width The width of a tile in the level (before scaling).
 * @param tile_height The height of a tile in the level (before scaling).
 * @param scaling_factor The scaling factor to apply to each tile.
 * @return The loaded level.
 */
Level *level_load(FILE *stream, const Tileset *tileset, int tile_width,
                  int tile_height, int scaling_factor);

/**
 * @brief Selects a level from a level hashmap based on its name and sets
 *          character's position to the spawn point of the selected level.
 *
 * @warning The selected level will be removed from the hashmap (levels).
 * @warning The current level (*current_level_ptr) will be destroyed if
 *              (*current_level_ptr) is not NULL.
 *
 * @param current_level_ptr Pointer to the current level. The current one will
 *                              be destroyed if it's not NULL and then will be
 *                              replaced with the new level.
 * @param levels Hashmap of Levels where the level to select is stored.
 *                  The selected level will be removed from it.
 * @param level_name The name of the level to select.
 * @param[out] character_hitbox_ptr Pointer to character's hitbox. The x
 *                                      and y of the hitbox will be changed to
 *                                      match the position of the spawn point
 *                                      on the selected level.
 *
 * @see level_destroy
 */
void level_select(Level **current_level_ptr, LevelHashmap *levels,
                  const char *level_name, SDL_FRect *character_hitbox_ptr);

/**
 * @brief Loads the levels stored in the given file paths using the given tileset.
 *
 * @param level_paths Paths to files with the level data.
 * @see level_load
 * @param size The size of the level_paths array.
 * @param tileset The tileset to use.
 * @param tile_width The width of a tile in the level (before scaling).
 * @param tile_height The height of a tile in the level (before scaling).
 * @param scaling_factor The scaling factor to apply to each tile.
 * @return Dynamically allocated hashmap from level name to the level itself.
 *
 * @see levels_unload
 */
LevelHashmap *levels_load(const char **level_paths, size_t size,
                          Tileset *tileset, int tile_width, int tile_height,
                          int scaling_factor);

/**
 * @brief Same as levels load, but instead of level paths, gets dir path, and finds the levels inside.
 *          The expected structure is:
 *              levels_dir_path/
 *              - level_group_1/
 *              - - level_1.csv
 *              - - level_2.csv
 *              - - ...
 *              - level_group_2/
 *              - - ...
 *              - ...
 *
 * @param levels_dir_path Path to the directory with the levels.
 * @param tileset The tileset to use.
 * @param tile_width The width of a tile in the level (before scaling).
 * @param tile_height The height of a tile in the level (before scaling).
 * @param scaling_factor The scaling factor to apply to each tile.
 * @return Dynamically allocated hashmap from level name to the level itself.
 *
 * @see levels_unload
 * @see levels_load
 */
LevelHashmap *levels_load_from_dirs(const char *levels_dir_path,
                                    Tileset *tileset, int tile_width,
                                    int tile_height, int scaling_factor);

/**
 * @brief Frees and cleans up the levels hashmap, and all the levels it stores.
 *
 * @param levels The levels hashmap to unload.
 *
 * @see level_destroy
 * @see levels_load
 */
void levels_unload(LevelHashmap *levels);

/**
 * @brief Checks for collision between all the tiles in the level with the given
 *          id (target_id) and the given hitbox.
 *
 * @param level The level to check for collision in.
 * @param target_id The id of the tile to check for collision.
 * @param hitbox The hitbox for which to check for collision.
 * @return True if there is collision, false otherwise.
 */
bool level_check_for_collision(Level *level, int target_id, SDL_FRect *hitbox);
