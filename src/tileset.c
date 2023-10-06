#include "SDL_image.h"
#include "csv.h"
#include "tile.h"
#include "tile_callback.h"
#include "tileset.h"
#include "utils.h"
#include "vec.h"
#include <stdbool.h>
#include <stdlib.h>

void tileset_entry_init(TilesetEntry *entry, int id, SDL_Texture *texture)
{
    entry->id = id;
    entry->texture = texture;
}

void tileset_entry_cleanup(TilesetEntry *entry)
{
    SDL_DestroyTexture(entry->texture);
    tile_callback_args_cleanup(&entry->args);
}

Tileset *tileset_create(char *texture_dir_path)
{
    Tileset *tileset = xmalloc(sizeof(*tileset));
    tileset->entries = vector_create();
    tileset->texture_dir_path = texture_dir_path;

    return tileset;
}

void tileset_destroy(Tileset *tileset)
{
    for (size_t i = 0; i < vector_size(tileset->entries); i++)
    {
        tileset_entry_cleanup(&tileset->entries[i]);
    }

    vector_free(tileset->entries);
    free(tileset->texture_dir_path);

    free(tileset);
}

/**
 * @brief Concatenates two paths.
 *
 * @param path1 The first path.
 * @param path2 The second path.
 * @param dir_separator The char used to separate the paths.
 * @return The concatenated path (dynamically allocated).
 */
char *concat_path(const char *path1, const char *path2,
                  const char path_separator)
{
    bool dir_sep_needed = path1[strlen(path1) - 1] != path_separator;

    char *path =
        xmalloc(strlen(path1) + (int)dir_sep_needed + strlen(path2) + 1);

    strcpy(path, path1);
    if (dir_sep_needed)
        strcat(path, (char[]){path_separator, '\0'});
    strcat(path, path2);

    return path;
}

// NOTE: if you prepend a type to the enum,
// you must modify `tileset_row_parser_callback` and `tileset_load`
enum FieldType
{
    FIELD_ID,
    FIELD_PATH,
    FIELD_CLASS_ID,
    FIELD_SOLID,
    FIELD_CALLBACK,
    FIELD_HITBOX_OFFSET_X,
    FIELD_HITBOX_OFFSET_Y,
};

struct TilesetLoadingData
{
    enum FieldType type;
    SDL_Renderer *renderer;
    Tileset *tileset;
};

typedef vec_char *vec_str;

/**
 * @brief Splits the given string on the given chars and returns
 *          a vector of tokens after split.
 *        The out vector is managed by the caller, both the vec and the
 *          vector and then strings (char vectors) inside.
 *
 * @param str The string to split.
 * @param ch The chars to split at.
 * @return Vector of strings (char vectors).
 *      All managed by the caller.
 */
vec_str str_split(const char *str, char ch)
{
    vec_str tokens = vector_create();

    vec_char curr_string = vector_create();

    do
    {
        if (*str == ch || *str == '\0')
        {
            vector_add(&curr_string, '\0');
            vector_add(&tokens, curr_string);
            curr_string = vector_create();
            continue;
        }

        vector_add(&curr_string, *str);
    } while (*str++);

    vector_free(curr_string);
    return tokens;
}

void tileset_field_parser_callback(void *field_bytes, size_t, void *data)
{
    struct TilesetLoadingData *tileset_loading_data = data;
    Tileset *tileset = tileset_loading_data->tileset;
    TilesetEntry *last_entry =
        &tileset->entries[vector_size(tileset->entries) - 1];

    // Field str will be null terminated (csv_parser option)
    const char *field_str = field_bytes;

    switch (tileset_loading_data->type)
    {
        case FIELD_ID:
            last_entry->id = atoi(field_str);
            break;
        case FIELD_CLASS_ID:
            last_entry->class_id = atoi(field_str);
            break;
        case FIELD_PATH:
        {
            char *texture_path = concat_path(tileset->texture_dir_path,
                                             field_str, DIR_SEPARATOR);

            last_entry->texture =
                IMG_LoadTexture(tileset_loading_data->renderer, texture_path);

            free(texture_path);
            break;
        }
        case FIELD_SOLID:
            last_entry->solid = atoi(field_str) != 0;
            break;
        case FIELD_CALLBACK:
        {
            vec_str callback_tokens = str_split(field_str, ' ');

            vec_char command_name = callback_tokens[0];
            const TileCallbackInfo *callback_info =
                tile_callback_get(command_name);

            SDL_assert(callback_info != NULL &&
                       "Tileset callback command not found");

            if (callback_info == NULL) // In case asserts are off.
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                            "Command '%s' in callback '%s' was not found. "
                            "Interpreting as command of type none.",
                            command_name, field_str);

                callback_info = tile_callback_get("");
            }

            TileCallbackFunction callback = callback_info->callback;
            SDL_assert(callback != NULL);

            TileArguments callback_args;
            if (vector_size(callback_tokens) > 1)
            {
                /* It's not a vec_str anymore, but a (vec_char *).
                 * Because we added +1, all the `vector_` functions will produce
                 * undefined behavior. */
                vec_char *command_args_tokens = callback_tokens + 1;
                size_t command_args_tokens_size =
                    vector_size(callback_tokens) - 1;

                switch (callback_info->type)
                {
                    case TILE_CALLBACK_NONE:
                    case TILE_CALLBACK_LADDER:
                        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                                    "The command '%s' has no arguments, but "
                                    "they were given in callback '%s'.",
                                    command_name, field_str);
                        break;
                }
            }
            callback_args.type = callback_info->type;

            for (size_t i = 0; i < vector_size(callback_tokens); i++)
            {
                vector_free(callback_tokens[i]);
            }
            vector_free(callback_tokens);

            last_entry->callback = callback;
            last_entry->args = callback_args;
            break;
        }
        case FIELD_HITBOX_OFFSET_X:
            last_entry->hitbox_offset.x = atoi(field_str);
            break;
        case FIELD_HITBOX_OFFSET_Y:
            last_entry->hitbox_offset.y = atoi(field_str);
            break;
    }

    // Change type to the next field type, as the enum is in order.
    // It will be reset back to the first type when row end is reached.
    tileset_loading_data->type++;
}

void tileset_row_parser_callback(int, void *data)
{
    struct TilesetLoadingData *tileset_loading_data = data;
    vector_add(&tileset_loading_data->tileset->entries, (TilesetEntry){0});
    tileset_loading_data->type = FIELD_ID; // Reset the type to the first one
}

Tileset *tileset_load(FILE *stream, char *texture_dir_path,
                      SDL_Renderer *renderer)
{
    struct csv_parser parser;
    if (csv_init(&parser, CSV_APPEND_NULL))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "csv_init()",
                                 csv_strerror(csv_error(&parser)), 0);
        return NULL;
    }

    tile_callback_init();

    /* It will work like this:
     * For each row, we append new entry to the vector.
     * For each field, we check what type of field it is, and write it.
     */
    struct TilesetLoadingData tileset_loading_data = {
        .renderer = renderer,
        .tileset = tileset_create(texture_dir_path),
        .type = FIELD_ID};
    vector_add(&tileset_loading_data.tileset->entries, (TilesetEntry){0});

    char buf[1024] = {0};
    size_t bytes_read = 0;
    while ((bytes_read = fread(buf, 1, sizeof(buf), stream)) > 0)
    {

        if (csv_parse(&parser, buf, bytes_read, tileset_field_parser_callback,
                      tileset_row_parser_callback,
                      &tileset_loading_data) != bytes_read)
        {
            tileset_destroy(tileset_loading_data.tileset);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                     "Error during level parsing",
                                     csv_strerror(csv_error(&parser)), 0);
            return NULL;
        }
    }

    csv_fini(&parser, tileset_field_parser_callback, tileset_row_parser_callback,
             &tileset_loading_data);
    csv_free(&parser);
    tile_callback_cleanup();

    // Pop the empty entry added by the loading algorithm
    // NOLINTNEXTLINE(bugprone-sizeof-expression)
    vector_remove(&tileset_loading_data.tileset->entries,
                  vector_size(tileset_loading_data.tileset->entries) - 1);

    return tileset_loading_data.tileset;
}

int tileset_query_texture_by_id(const Tileset *tileset, int id,
                                SDL_Texture **texture,
                                SDL_FPoint *hitbox_offset, bool *solid,
                                TileCallback *callback, int *class_id)
{
    TilesetEntry *entries_end =
        tileset->entries + vector_size(tileset->entries);

    for (TilesetEntry *entry = tileset->entries; entry < entries_end; entry++)
    {
        if (entry->id == id)
        {
            if (texture)
                *texture = entry->texture;
            if (solid)
                *solid = entry->solid;
            if (callback)
                *callback = (TileCallback){entry->callback, &entry->args, id};
            if (class_id)
                *class_id = entry->class_id;
            if (hitbox_offset)
                *hitbox_offset = entry->hitbox_offset;

            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}
