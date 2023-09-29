#include "SDL.h"
#include "csv.h"
#include "dir.h"
#include "level.h"
#include "level_layer.h"
#include "tile.h"
#include "tileset.h"
#include "utils.h"
#include "vec.h"
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

Level *level_create(char *name)
{
    Level *level = xmalloc(sizeof(*level));

    level->layers = vector_create();
    level->name = name;

    return level;
}

void level_destroy(Level *level)
{
    for (size_t i = 0; i < vector_size(level->layers); i++)
    {
        level_layer_destroy(level->layers[i]);
    }
    vector_free(level->layers);
    free(level->name);
    free(level);
}

void level_draw(const Level *level, SDL_Renderer *renderer, SDL_FPoint *offset)
{
    for (size_t i = 0; i < vector_size(level->layers); i++)
    {
        level_layer_draw(level->layers[i], renderer, offset);
    }
}

void level_add_layer(Level *level, LevelLayer *layer)
{
    // NOLINTNEXTLINE(bugprone-sizeof-expression)
    vector_add(&level->layers, layer);
}

bool level_check_for_collision(Level *level, int target_id, SDL_FRect *hitbox)
{
    LevelLayer *level_layer;
    vector_foreach(level_layer, level->layers)
    {
        vector_iter(tile, level_layer->tiles)
        {
            if (tile->texture_id != target_id)
                continue;

            if (SDL_HasIntersectionF(hitbox, &tile->hitbox))
                return true;
        }
    }

    return false;
}

/**
 * @brief Returns the index of ch in the array
 *
 * @param arr The array
 * @param size Array's size
 * @return The index of ch or `size` if ch is not in the array.
 */
size_t index_of(char *arr, size_t size, char ch)
{
    for (size_t i = 0; i < size; i++)
    {
        if (arr[i] == ch)
            return i;
    }

    return size;
}

struct LayerLoadingData
{
    const int tile_width, tile_height;
    SDL_Point current_pos;
    LevelLayer *current_layer;
    const Tileset *tileset;
    int scaling_factor;
};

void level_field_parser_callback(void *field_bytes, size_t, void *data)
{
    struct LayerLoadingData *layer_loading_data = data;

    // Field str will be null terminated (csv_parser option)
    const char *field_str = field_bytes;
    int id = atoi(field_str);

    int w = 0, h = 0;
    SDL_Texture *tile_texture = NULL;
    bool solid = 0;
    TileCallback callback;
    if (tileset_query_texture_by_id(layer_loading_data->tileset, id,
                                    &tile_texture, &solid, &callback))
    {
        die("Error while loading level:\nNo texture with ID %d", id);
    }

    if (!SDL_QueryTexture(tile_texture, NULL, NULL, &w, &h))
    {
        Tile tile;
        tile_init(&tile,
                  (SDL_FRect){layer_loading_data->current_pos.x,
                              layer_loading_data->current_pos.y,
                              w * layer_loading_data->scaling_factor,
                              h * layer_loading_data->scaling_factor},
                  tile_texture, callback, id, solid);
        level_layer_add_tile(layer_loading_data->current_layer, tile);
    }

    layer_loading_data->current_pos.x +=
        layer_loading_data->tile_width * layer_loading_data->scaling_factor;
}

void level_row_parser_callback(int, void *data)
{
    struct LayerLoadingData *layer_loading_data = data;
    layer_loading_data->current_pos.x = 0;
    layer_loading_data->current_pos.y +=
        layer_loading_data->tile_height * layer_loading_data->scaling_factor;
}

Level *level_load(FILE *stream, const Tileset *tileset, int tile_width,
                  int tile_height, int scaling_factor)
{
    struct csv_parser parser;
    if (csv_init(&parser, CSV_APPEND_NULL))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "csv_init()",
                                 csv_strerror(csv_error(&parser)), 0);
        return NULL;
    }

    char *level_name = readline(stream, '\n', 0, 0);
    Level *level = level_create(level_name);

    struct LayerLoadingData layer_loading_data = {
        .tile_width = tile_width,
        .tile_height = tile_height,
        .tileset = tileset,
        .scaling_factor = scaling_factor,
        .current_pos = (SDL_Point){0},
        .current_layer = level_layer_create()};

    char buf[1024] = {0};
    size_t bytes_read = 0;
    while ((bytes_read = fread(buf, 1, sizeof(buf), stream)) > 0)
    {
        char *layer_buf = buf;
        size_t layer_size = 0;
        /*
         * Note about the following algorithm: We do not care if layer separator
         * comes with a new line, as csv_parse ignores empty lines by default.
         *
         * The algo works by just reading until the sep, and moving the layer
         * buffer to start right after the sep. And we read like this until we
         * have read the whole buf.
         */
        do
        {
            size_t prev_layer_size = layer_size;

            layer_buf += prev_layer_size;
            layer_size = index_of(layer_buf, bytes_read - prev_layer_size,
                                  LEVEL_LAYER_SEPARATOR);

            if (csv_parse(&parser, layer_buf, layer_size,
                          level_field_parser_callback,
                          level_row_parser_callback,
                          &layer_loading_data) != layer_size)
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                         "Error during level parsing",
                                         csv_strerror(csv_error(&parser)), 0);
            }

            bool there_will_be_a_new_layer =
                layer_size != bytes_read - prev_layer_size;
            if (there_will_be_a_new_layer)
            {
                level_add_layer(level, layer_loading_data.current_layer);
                layer_loading_data.current_layer = level_layer_create();
                layer_loading_data.current_pos = (SDL_Point){0};
                layer_size += 1; // Skip the separator.
            }

        } while (layer_buf + layer_size != buf + bytes_read);
    }

    csv_fini(&parser, level_field_parser_callback, level_row_parser_callback,
             &layer_loading_data);
    csv_free(&parser);

    level_add_layer(level, layer_loading_data.current_layer);

    return level;
}

typedef const char *vec_const_char;
typedef vec_const_char *vec_const_str;

LevelHashmap *levels_load_from_dirs(const char *levels_dir_path,
                                    Tileset *tileset, int tile_width,
                                    int tile_height, int scaling_factor)
{
    vec_const_str level_paths = vector_create();

    DIR *levels_dir = opendir(levels_dir_path);
    if (!levels_dir)
        die("Opening directory %s failed", levels_dir_path);

    struct dirent *levels_dir_entry;
    while ((levels_dir_entry = readdir(levels_dir)))
    {
        if (strcmp(levels_dir_entry->d_name, ".") == 0 ||
            strcmp(levels_dir_entry->d_name, "..") == 0)
            continue;

        char *level_dir_path =
            dir_get_path_to_entry(levels_dir_entry, levels_dir_path);

        DIR *level_dir = opendir(level_dir_path);
        if (!level_dir)
            die("Opening directory %s failed", level_dir_path);

        struct dirent *level_dir_entry;
        while ((level_dir_entry = readdir(level_dir)))
        {
            if (strcmp(level_dir_entry->d_name, ".") == 0 ||
                strcmp(level_dir_entry->d_name, "..") == 0)
                continue;

            const char *level_path =
                dir_get_path_to_entry(level_dir_entry, level_dir_path);

            vector_add(&level_paths, level_path);
        }

        free(level_dir_path);
        closedir(level_dir);
    }

    closedir(levels_dir);

    LevelHashmap *levels =
        levels_load(level_paths, vector_size(level_paths), tileset, tile_width,
                    tile_height, scaling_factor);

    const char *path;
    vector_foreach(path, level_paths)
    {
        free((char *)path);
    }

    vector_free(level_paths);

    return levels;
}

LevelHashmap *levels_load(const char **level_paths, size_t size,
                          Tileset *tileset, int tile_width, int tile_height,
                          int scaling_factor)
{
    LevelHashmap *levels = malloc(sizeof(*levels));
    hashmap_init(levels, hashmap_hash_string, strcmp);

    for (size_t i = 0; i < size; i++)
    {
        FILE *file = fopen(level_paths[i], "rb");

        if (!file)
            die("Opening file %s failed", level_paths[i]);

        Level *level =
            level_load(file, tileset, tile_width, tile_height, scaling_factor);

        fclose(file);

        int err = hashmap_put(levels, level->name, level);

        if (err == -EEXIST)
            die("Names of levels should be unique, but %s appeared more than "
                "once",
                level->name);
        else if (err)
            die("Error while loading level %s - %s", level, strerror(-err));
    }

    return levels;
}

void levels_unload(LevelHashmap *levels)
{
    const char *key;
    void *temp;

    hashmap_foreach_key_safe(key, levels, temp)
    {
        Level *level = hashmap_remove(levels, key);
        level_destroy(level);
    }

    hashmap_cleanup(levels);
    free(levels);
}
