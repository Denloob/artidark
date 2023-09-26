#include "hashmap.h"
#include "tile_keyboard_events.h"
#include "utils.h"
#include "vec.h"
#include <csv.h>

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

// NOTE: if you prepend a type to the enum,
// you must modify `tile_keyboard_mapping_parser_row_callback` and `tile_keyboard_mappings_load`
enum FieldType
{
    FIELD_CLASS_ID,
    FIELD_KEY,
};

struct MappingEntry
{
    int class_id;
    int key;
};

struct MappingLoadingData
{
    enum FieldType type;
    Tileset *tileset;
    KeyEventSubscribers *subscribers;
    struct MappingEntry map_entry;
};

void tile_keyboard_mapping_parser_row_callback(int, void *data)
{
    struct MappingLoadingData *loading_data = data;
    struct MappingEntry *map_entry = &loading_data->map_entry;

    loading_data->type = FIELD_CLASS_ID;

    VecTilesetEntry tileset_entries = loading_data->tileset->entries;
    vector_iter(tileset_entry, tileset_entries)
    {
        if (tileset_entry->class_id != map_entry->class_id)
            continue;

        tile_keyboard_events_subscribe(loading_data->subscribers,
                                       map_entry->key, loading_data->tileset,
                                       tileset_entry->id);
    }
}

void tile_keyboard_mapping_parser_field_callback(void *field_bytes, size_t,
                                                 void *data)
{
    struct MappingLoadingData *loading_data = data;
    struct MappingEntry *map_entry = &loading_data->map_entry;

    // Field str will be null terminated (csv_parser option)
    const char *field_str = field_bytes;

    switch (loading_data->type)
    {
        case FIELD_CLASS_ID:
            map_entry->class_id = atoi(field_str);
            break;
        case FIELD_KEY:
            map_entry->key = atoi(field_str);
            break;
    }

    // Change type to the next field type, as the enum is in order.
    // It will be reset back to the first type when row end is reached.
    loading_data->type++;
}

KeyEventSubscribers *tile_keyboard_mappings_load(FILE *stream, Tileset *tileset)
{
    struct csv_parser parser;
    if (csv_init(&parser, CSV_APPEND_NULL))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "csv_init()",
                                 csv_strerror(csv_error(&parser)), 0);
        return NULL;
    }

    struct MappingLoadingData mapping_loading_data = {
        .tileset = tileset,
        .subscribers = tile_keyboard_events_create(),
        .type = FIELD_CLASS_ID,
    };

    char buf[1024] = {0};
    size_t bytes_read = 0;
    while ((bytes_read = fread(buf, 1, sizeof(buf), stream)) > 0)
    {

        if (csv_parse(&parser, buf, bytes_read,
                      tile_keyboard_mapping_parser_field_callback,
                      tile_keyboard_mapping_parser_row_callback,
                      &mapping_loading_data) != bytes_read)
        {
            tile_keyboard_events_destroy(mapping_loading_data.subscribers);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                     "Error during keymap parsing",
                                     csv_strerror(csv_error(&parser)), 0);
            return NULL;
        }
    }

    csv_fini(&parser, tile_keyboard_mapping_parser_field_callback,
             tile_keyboard_mapping_parser_row_callback, &mapping_loading_data);
    csv_free(&parser);

    return mapping_loading_data.subscribers;
}
