#include "SDL.h"
#include "character.h"
#include "hashmap.h"
#include "level.h"
#include "string.h"
#include "tile_callback.h"
#include "utils.h"
#include "vec.h"
#include <stdlib.h>

typedef HASHMAP(char, TileCallbackInfo) TileCallbackHashmap;

static TileCallbackHashmap tile_callbacks;

TileCallbackInfo *tile_callback_info_create(const char *name,
                                            TileCallbackType type,
                                            TileCallbackFunction callback)
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
                       TileCallbackFunction callback)
{
    TileCallbackInfo *info = tile_callback_info_create(name, type, callback);

    hashmap_put(&tile_callbacks, info->name, info);
}

void tile_callback_init()
{
    hashmap_init(&tile_callbacks, hashmap_hash_string, strcmp);

    tile_callback_add("", TILE_CALLBACK_NONE, tile_callback_none);
    tile_callback_add("door", TILE_CALLBACK_DOOR, tile_callback_door);
}

void tile_callback_cleanup()
{
    const char *key;
    void *temp;

    // The reason for the warning "Missing field 'iter_types' initializer" is a
    // 0 width array "not" being initialized.
    hashmap_foreach_key_safe(key, &tile_callbacks, temp)
    {
        TileCallbackInfo *info = hashmap_remove(&tile_callbacks, key);
        tile_callback_info_destroy(info);
    }

    hashmap_cleanup(&tile_callbacks);
}

const TileCallbackInfo *tile_callback_get(const char *name)
{
    return hashmap_get(&tile_callbacks, name);
}

void tile_callback_door(TileArguments *args, CallbackGameState *game_state)
{
    SDL_assert(args->type == TILE_CALLBACK_DOOR);
    struct TileCallbackDoorArgument *door = &args->door;

    Level *level = *game_state->level_ptr;

    SDL_FRect *character_hitbox = &game_state->character->hitbox;

    if (level_check_for_collision(level, game_state->tile_texture_id,
                                   character_hitbox))
        SDL_Log("The character interacted with the door that goes to %s",
                door->destination_level);

    // TODO: not implemented
}

void tile_callback_none(TileArguments *, CallbackGameState *)
{
    /* Does nothing. */
}

void tile_callback_door_cleanup(struct TileCallbackDoorArgument *args)
{
    free(args->destination_level);
}

void tile_callback_args_cleanup(TileArguments *args)
{
    switch (args->type)
    {
        case TILE_CALLBACK_NONE:
            break;
        case TILE_CALLBACK_DOOR:
            tile_callback_door_cleanup(&args->door);
    }
}
