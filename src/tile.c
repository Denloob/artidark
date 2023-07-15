#include "SDL.h"
#include "tile.h"
#include <stdlib.h>

Tile *tile_create(SDL_FRect hitbox, bool solid)
{
    Tile *tile = malloc(sizeof(*tile));

    tile->hitbox = hitbox;
    tile->solid = solid;

    return tile;
}

void tile_destroy(Tile *tile)
{
    free(tile);
}

void tile_draw(const Tile *tile, SDL_Renderer *renderer)
{
    SDL_RenderDrawRectF(renderer, &tile->hitbox);
}
