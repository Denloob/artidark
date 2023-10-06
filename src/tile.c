#include "SDL.h"
#include "renderer.h"
#include "tile.h"
#include "tile_callback.h"

void tile_draw(const Tile *tile, SDL_Renderer *renderer, SDL_FPoint *offset)
{
    renderer_render_copy_with_offset_f(renderer, tile->texture, NULL,
                                       &tile->hitbox, offset);
}

void tile_init(Tile *tile, SDL_FRect hitbox, SDL_Texture *texture,
               TileCallback callback, int texture_id, int class_id, bool solid)
{
    tile->texture = texture;
    tile->texture_id = texture_id;
    tile->class_id = class_id;

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
    tile->callback = callback;
}
