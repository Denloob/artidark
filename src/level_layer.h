#pragma once

#include "SDL.h"
#include "tile.h"
#include "vec.h"

typedef struct LevelLayer
{
    VecTile tiles;
} LevelLayer;

typedef LevelLayer **VecLevelLayer;

/**
 * @brief Creates a level layer.
 *
 * @return The created level layer.
 */
LevelLayer *level_layer_create(void);

/**
 * @brief Destroys the level layer.
 */
void level_layer_destroy(LevelLayer *layer);

/**
 * @brief Adds a tile to the level layer.
 *
 * @param tile The tile to add.
 */
void level_layer_add_tile(LevelLayer *layer, Tile tile);

/**
 * @brief Draws the level layer.
 *
 * @param renderer The renderer to draw onto.
 * @param offset The offset to apply to each of the tiles.
 *
 * @see tile_draw
 */
void level_layer_draw(const LevelLayer *layer, SDL_Renderer *renderer,
                      SDL_FPoint *offset);
