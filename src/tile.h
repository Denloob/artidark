#pragma once

#include "SDL.h"
#include <stdbool.h>

typedef struct Tile
{
    SDL_FRect hitbox;
    bool solid;
} Tile;

typedef Tile *VecTile;

/**
 * @brief Creates a tile
 *
 * @param hitbox The hitbox of the tile
 * @param solid Whether or not the tile is solid
 * @return The created tile
 */
Tile *tile_create(SDL_FRect hitbox, bool solid);

/**
 * @brief Destroys the tile
 *
 * @param tile The tile to destroy
 */
void tile_destroy(Tile *tile);

/**
 * @brief Draws the tile.
 *
 * @param renderer The renderer to draw onto.
 */
void tile_draw(const Tile *tile, SDL_Renderer *renderer);
