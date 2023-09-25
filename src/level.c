#include "SDL.h"
#include "level.h"
#include "level_layer.h"
#include "tile.h"
#include "tileset.h"
#include "utils.h"
#include "vec.h"
#include <csv.h>
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

void level_addLayer(Level *level, LevelLayer *layer)
{
    // NOLINTNEXTLINE(bugprone-sizeof-expression)
    vector_add(&level->layers, layer);
}

/**
 * @brief Returns the index of ch in the array
 *
 * @param arr The array
 * @param size Array's size
 * @return The index of ch or `size` if ch is not in the array.
 */
size_t indexOf(char *arr, size_t size, char ch)
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
    const int tileWidth, tileHeight;
    SDL_Point currentPos;
    LevelLayer *currentLayer;
    const Tileset *tileset;
    int scalingFactor;
};

void level_fieldParserCallback(void *fieldBytes,
                               size_t _ __attribute__((unused)), void *data)
{
    struct LayerLoadingData *layerLoadingData = data;

    // Field str will be null terminated (csv_parser option)
    const char *fieldStr = fieldBytes;
    int id = atoi(fieldStr);

    int w = 0, h = 0;
    SDL_Texture *tileTexture = NULL;
    bool solid = 0;
    TileCallback callback;
    if (tileset_QueryTextureByID(layerLoadingData->tileset, id, &tileTexture,
                                 &solid, &callback))
    {
        die("Error while loading level:\nNo texture with ID %d", id);
    }

    if (!SDL_QueryTexture(tileTexture, NULL, NULL, &w, &h))
    {
        Tile tile;
        tile_init(&tile,
                  (SDL_FRect){layerLoadingData->currentPos.x,
                              layerLoadingData->currentPos.y,
                              w * layerLoadingData->scalingFactor,
                              h * layerLoadingData->scalingFactor},
                  tileTexture, callback, id, solid);
        level_layer_add_tile(layerLoadingData->currentLayer, tile);
    }

    layerLoadingData->currentPos.x +=
        layerLoadingData->tileWidth * layerLoadingData->scalingFactor;
}

void level_rowParserCallback(int _ __attribute__((unused)), void *data)
{
    struct LayerLoadingData *layerLoadingData = data;
    layerLoadingData->currentPos.x = 0;
    layerLoadingData->currentPos.y +=
        layerLoadingData->tileHeight * layerLoadingData->scalingFactor;
}

Level *level_load(FILE *stream, const Tileset *tileset, int tileWidth,
                  int tileHeight, int scalingFactor)
{
    struct csv_parser parser;
    if (csv_init(&parser, CSV_APPEND_NULL))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "csv_init()",
                                 csv_strerror(csv_error(&parser)), 0);
        return NULL;
    }

    char *levelName = readline(stream, '\n', 0, 0);
    Level *level = level_create(levelName);

    struct LayerLoadingData layerLoadingData = {.tileWidth = tileWidth,
                                                .tileHeight = tileHeight,
                                                .tileset = tileset,
                                                .scalingFactor = scalingFactor,
                                                .currentPos = (SDL_Point){0},
                                                .currentLayer =
                                                    level_layer_create()};

    char buf[1024] = {0};
    size_t bytesRead = 0;
    while ((bytesRead = fread(buf, 1, sizeof(buf), stream)) > 0)
    {
        char *layerBuf = buf;
        size_t layerSize = 0;
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
            size_t prevLayerSize = layerSize;

            layerBuf += prevLayerSize;
            layerSize = indexOf(layerBuf, bytesRead - prevLayerSize,
                                LEVEL_LAYER_SEPARATOR);

            if (csv_parse(&parser, layerBuf, layerSize,
                          level_fieldParserCallback, level_rowParserCallback,
                          &layerLoadingData) != layerSize)
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                         "Error during level parsing",
                                         csv_strerror(csv_error(&parser)), 0);
            }

            bool thereWillBeANewLayer = layerSize != bytesRead - prevLayerSize;
            if (thereWillBeANewLayer)
            {
                level_addLayer(level, layerLoadingData.currentLayer);
                layerLoadingData.currentLayer = level_layer_create();
                layerLoadingData.currentPos = (SDL_Point){0};
                layerSize += 1; // Skip the separator.
            }

        } while (layerBuf + layerSize != buf + bytesRead);
    }

    csv_fini(&parser, level_fieldParserCallback, level_rowParserCallback,
             &layerLoadingData);
    csv_free(&parser);

    level_addLayer(level, layerLoadingData.currentLayer);

    return level;
}
