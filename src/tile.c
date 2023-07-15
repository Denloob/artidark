#include "SDL.h"
#include "tile.h"
#include <stdlib.h>

void tile_draw(const Tile *tile, SDL_Renderer *renderer)
{
    SDL_RenderDrawRectF(renderer, &tile->hitbox);
}
