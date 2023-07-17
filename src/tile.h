#pragma once

#include "SDL.h"
#include <stdbool.h>

typedef struct Tile
{
    SDL_FRect hitbox;
    SDL_Texture *texture;
    bool solid;
} Tile;

typedef Tile *VecTile;

/**
 * @brief Initializes a tile
 *
 * @param tile The tile to initialize.
 * @param hitbox The hitbox of the tile. If width or height are 0, they are
 *                  determined by the texture.
 * @param texture The texture of the tile.
 * @param solid Whether the tile is solid or not.
 */
void tile_init(Tile *tile, SDL_FRect hitbox, SDL_Texture *texture, bool solid);

// There is no clean-up as the idea is that the textures are shared between
// tiles, and texture is the only thing that needs clean up.

/**
 * @brief Draws the tile.
 *
 * @param renderer The renderer to draw onto.
 */
void tile_draw(const Tile *tile, SDL_Renderer *renderer);
