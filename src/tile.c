#include "SDL.h"
#include "hashmap.h"
#include "renderer.h"
#include "tile.h"
#include "utils.h"
#include <stdlib.h>

typedef HASHMAP(char, TileCallbackInfo) TileCallbackHashmap;

static TileCallbackHashmap tileCallbacks;

TileCallbackInfo *tile_callback_info_create(const char *name,
                                            TileCallbackType type,
                                            TileCallback callback)
{
    TileCallbackInfo *info = xmalloc(sizeof(TileCallbackInfo));
    info->name = strdup(name);
    info->type = type;
    info->callback = callback;

    return info;
}

void tile_callback_info_destroy(TileCallbackInfo *info)
{
    free(info->name);
    free(info);
}

void tile_callback_add(const char *name, TileCallbackType type,
                       TileCallback callback)
{
    TileCallbackInfo *info = tile_callback_info_create(name, type, callback);

    hashmap_put(&tileCallbacks, info->name, info);
}

void tile_callback_init()
{
    hashmap_init(&tileCallbacks, hashmap_hash_string, strcmp);

    tile_callback_add("", TILE_CALLBACK_NONE, tile_callback_none);
}

void tile_callback_cleanup()
{
    const char *key;
    void *temp;

    // The reason for the warning "Missing field 'iter_types' initializer" is a
    // 0 width array "not" being initialized.
    hashmap_foreach_key_safe(key, &tileCallbacks, temp)
    {
        TileCallbackInfo *info = hashmap_remove(&tileCallbacks, key);
        tile_callback_info_destroy(info);
    }

    hashmap_cleanup(&tileCallbacks);
}

const TileCallbackInfo *tile_callback_get(const char *name)
{
    return hashmap_get(&tileCallbacks, name);
}

void tile_callback_none(TileArguments *)
{
    /* Does nothing. */
}

void tile_callback_args_cleanup(TileArguments *args)
{
    switch (args->type)
    {
        case TILE_CALLBACK_NONE:
            break;
    }
}

void tile_draw(const Tile *tile, SDL_Renderer *renderer, SDL_FPoint *offset)
{
    renderer_renderCopyWithOffsetF(renderer, tile->texture, NULL, &tile->hitbox,
                                   offset);
}

void tile_init(Tile *tile, SDL_FRect hitbox, SDL_Texture *texture, bool solid, TileCallback callback, TileArguments *args)
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
    tile->callback = callback;
    tile->args = args;
}
