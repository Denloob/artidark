#include "hashmap.h"
#include "tile_keyboard_events.h"
#include "utils.h"
#include "vec.h"

static size_t tile_keyboard_hash_SDL_Keycode(const SDL_Keycode *key)
{
    return hashmap_hash_default(key, sizeof(*key));
}

static int tile_keyboard_compare_SDL_Keycode(const SDL_Keycode *a,
                                             const SDL_Keycode *b)
{
    return memcmp(a, b, sizeof(*a));
}

static SDL_Keycode *tile_keyboard_copy_SDL_Keycode(const SDL_Keycode *key)
{
    SDL_Keycode *key_dup = xmalloc(sizeof(*key));
    memcpy(key_dup, key, sizeof(*key));
    return key_dup;
}

static void tile_keyboard_free_SDL_Keycode(SDL_Keycode *key)
{
    free(key);
}

KeyEventSubscribers *tile_keyboard_events_create()
{
    KeyEventSubscribers *subscribers = xmalloc(sizeof(*subscribers));
    hashmap_init(subscribers, tile_keyboard_hash_SDL_Keycode,
                 tile_keyboard_compare_SDL_Keycode);
    hashmap_set_key_alloc_funcs(subscribers, tile_keyboard_copy_SDL_Keycode,
                                tile_keyboard_free_SDL_Keycode);

    return subscribers;
}

void tile_keyboard_events_destroy(KeyEventSubscribers *subscribers)
{
    vec_TileCallback subscribed_callbacks;

    hashmap_foreach_data(subscribed_callbacks, subscribers)
    {
        TileCallback *callback;
        vector_foreach(callback, subscribed_callbacks)
        {
            free(callback);
        }

        vector_free(subscribed_callbacks);
    }

    hashmap_cleanup(subscribers);
    free(subscribers);
}

void tile_keyboard_events_subscribe_dynamic(KeyEventSubscribers *subscribers,
                                            SDL_Keycode key,
                                            TileCallback *callback)
{
    vec_TileCallback subscribed_callbacks = hashmap_remove(subscribers, &key);
    if (!subscribed_callbacks)
    {
        subscribed_callbacks = vector_create();
    }

    // NOLINTNEXTLINE(bugprone-sizeof-expression)
    vector_add(&subscribed_callbacks, callback);

    hashmap_put(subscribers, &key, subscribed_callbacks);
}

void tile_keyboard_events_unsubscribe(KeyEventSubscribers *subscribers,
                                      SDL_Keycode key, TileCallback *callback)
{
    vec_TileCallback subscribed_callbacks = hashmap_remove(subscribers, &key);

    if (!subscribed_callbacks)
        return;

    for (size_t i = 0; i < vector_size(subscribed_callbacks); i++)
    {
        if (subscribed_callbacks[i] == callback)
        {
            vector_remove(&subscribed_callbacks, i);
            break;
        }
    }

    hashmap_put(subscribers, &key, subscribed_callbacks);
}

void tile_keyboard_events_notify(KeyEventSubscribers *subscribers,
                                 SDL_Keycode key, CallbackGameState *game_info)
{
    game_info->key = key;
    TileCallback **subscribed_callbacks = hashmap_get(subscribers, &key);

    if (!subscribed_callbacks)
        return;

    TileCallback *callback;
    vector_foreach(callback, subscribed_callbacks)
    {
        game_info->tile_texture_id = callback->id;

        callback->func(callback->args, game_info);
    }
}
