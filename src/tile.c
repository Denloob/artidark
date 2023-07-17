#include "SDL.h"
#include "tile.h"
#include <stdlib.h>

void tile_draw(const Tile *tile, SDL_Renderer *renderer)
{
    SDL_RenderCopyF(renderer, tile->texture, NULL, &tile->hitbox);
}

void tile_init(Tile *tile, SDL_FRect hitbox, SDL_Texture *texture, bool solid)
{
    tile->texture = texture;

    if (hitbox.w == 0 || hitbox.h == 0)
    {
        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);

        if (hitbox.w == 0)
            hitbox.w = w;
        if (hitbox.h == 0)
            hitbox.h = h;
    }

    tile->hitbox = hitbox;

    tile->solid = solid;
}
