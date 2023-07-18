#include "SDL_image.h"
#include "csv.h"
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
    free(entry->texture);
}

Tileset *tileset_create(char *textureDirPath)
{
    Tileset *tileset = xmalloc(sizeof(*tileset));
    tileset->entries = vector_create();
    tileset->textureDirPath = textureDirPath;

    return tileset;
}

void tileset_destroy(Tileset *tileset)
{
    for (size_t i = 0; i < vector_size(tileset->entries); i++)
    {
        tileset_entry_cleanup(&tileset->entries[i]);
    }

    vector_free(tileset->entries);

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
char *concatPath(const char *path1, const char *path2, const char pathSeparator)
{
    bool dirSepNeeded = path1[strlen(path1) - 1] != pathSeparator;

    char *path = xmalloc(strlen(path1) + (int)dirSepNeeded + strlen(path2) + 1);

    strcpy(path, path1);
    if (dirSepNeeded)
        strcat(path, (char[]){pathSeparator, '\0'});
    strcat(path, path2);

    return path;
}

// NOTE: if you prepend a type to the enum,
// you must modify `row_parser_callback`
enum FieldType
{
    FIELD_ID,
    FIELD_PATH,
    FIELD_SOLID,
};

struct TilesetLoadingData
{
    enum FieldType type;
    SDL_Renderer *renderer;
    Tileset *tileset;
};

void tileset_fieldParserCallback(void *fieldBytes,
                                   size_t _ __attribute__((unused)), void *data)
{

    struct TilesetLoadingData *tilesetLoadingData = data;
    Tileset *tileset = tilesetLoadingData->tileset;
    TilesetEntry *lastEntry =
        &tileset->entries[vector_size(tileset->entries) - 1];

    // Field str will be null terminated (csv_parser option)
    const char *fieldStr = fieldBytes;

    switch (tilesetLoadingData->type)
    {
        case FIELD_ID:
            lastEntry->id = atoi(fieldStr);
            break;
        case FIELD_PATH:
        {
            char *texturePath =
                concatPath(tileset->textureDirPath, fieldStr, DIR_SEPARATOR);

            lastEntry->texture =
                IMG_LoadTexture(tilesetLoadingData->renderer, texturePath);

            free(texturePath);
            break;
        }
        case FIELD_SOLID:
            lastEntry->solid = atoi(fieldStr) != 0;
            break;
    }

    // Change type to the next field type, as the enum is in order.
    // It will be reset back to the first type when row end is reached.
    tilesetLoadingData->type++;
}

void tileset_rowParserCallback(int _ __attribute__((unused)), void *data)
{
    struct TilesetLoadingData *tilesetLoadingData = data;
    vector_add(&tilesetLoadingData->tileset->entries, (TilesetEntry){0});
    tilesetLoadingData->type = FIELD_ID; // Reset the type to the first one
}

Tileset *tileset_load(FILE *stream, char *textureDirPath,
                      SDL_Renderer *renderer)
{
    struct csv_parser parser;
    if (csv_init(&parser, CSV_APPEND_NULL))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "csv_init()",
                                 csv_strerror(csv_error(&parser)), 0);
        return NULL;
    }

    /* It will work like this:
     * For each row, we append new entry to the vector.
     * For each field, we check what type of field it is, and write it.
     */
    struct TilesetLoadingData tilesetLoadingData = {
        .renderer = renderer,
        .tileset = tileset_create(textureDirPath),
        .type = FIELD_ID};
    vector_add(&tilesetLoadingData.tileset->entries, (TilesetEntry){0});

    char buf[1024] = {0};
    size_t bytesRead = 0;
    while ((bytesRead = fread(buf, 1, sizeof(buf), stream)) > 0)
    {

        if (csv_parse(&parser, buf, bytesRead, tileset_fieldParserCallback,
                      tileset_rowParserCallback,
                      &tilesetLoadingData) != bytesRead)
        {
            tileset_destroy(tilesetLoadingData.tileset);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                     "Error during level parsing",
                                     csv_strerror(csv_error(&parser)), 0);
            return NULL;
        }
    }

    csv_fini(&parser, tileset_fieldParserCallback,
             tileset_rowParserCallback, &tilesetLoadingData);
    csv_free(&parser);

    // Pop the empty entry added by the loading algorithm
    // NOLINTNEXTLINE(bugprone-sizeof-expression)
    vector_remove(&tilesetLoadingData.tileset->entries,
                  vector_size(tilesetLoadingData.tileset->entries) - 1);

    return tilesetLoadingData.tileset;
}

int tileset_QueryTextureByID(const Tileset *tileset, int id,
                             SDL_Texture **texture, bool *solid)
{
    for (size_t i = 0; i < vector_size(tileset->entries); i++)
    {
        if (tileset->entries[i].id == id)
        {
            if (texture)
                *texture = tileset->entries[i].texture;
            if (solid)
                *solid = tileset->entries[i].solid;

            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}
