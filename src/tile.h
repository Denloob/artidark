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
 * @brief Draws the tile.
 *
 * @param renderer The renderer to draw onto.
 */
void tile_draw(const Tile *tile, SDL_Renderer *renderer);
