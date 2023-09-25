#pragma once

#include "SDL.h"
#include "tile_callback.h"
#include <stdbool.h>

typedef struct Tile
{
    SDL_FRect hitbox;
    SDL_Texture *texture;
    TileCallback callback;
    int texture_id;
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
 * @param callback The callback for the tile.
 * @param texture_id The id of the texture.
 * @param solid Whether the tile is solid or not.
 */
void tile_init(Tile *tile, SDL_FRect hitbox, SDL_Texture *texture,
               TileCallback callback, int texture_id, bool solid);

// There is no clean-up as there is no memory managed by the tile.

/**
 * @brief Draws the tile.
 *
 * @param renderer The renderer to draw onto.
 * @param offset The offset to apply to the tile's x and y.
 *
 * @see renderer_renderCopyWithOffsetF
 */
void tile_draw(const Tile *tile, SDL_Renderer *renderer, SDL_FPoint *offset);
