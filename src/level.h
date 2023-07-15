#pragma once

#include "SDL.h"
#include "tile.h"

typedef struct Level
{
    VecTile tiles;
} Level;

/**
 * @brief Creates a level.
 *
 * @return The created level.
 */
Level *level_create(void);

/**
 * @brief Destroys the level.
 *
 * @param level The level to destroy.
 */
void level_destroy(Level *level);

/**
 * @brief Adds a tile to the level.
 *
 * @param tile The tile to add.
 */
void level_add_tile(Level *level, Tile tile);

/**
 * @brief Draws the level.
 *
 * @param renderer The renderer to draw onto.
 */
void level_draw(const Level *level, SDL_Renderer *renderer);
