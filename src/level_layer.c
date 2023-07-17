#include "SDL.h"
#include "level_layer.h"
#include "utils.h"
#include "vec.h"

LevelLayer *level_layer_create(void)
{
    LevelLayer *layer = xmalloc(sizeof(*layer));

    layer->tiles = vector_create();

    return layer;
}

void level_layer_destroy(LevelLayer *layer)
{
    vector_free(layer->tiles);
    free(layer);
}

void level_layer_add_tile(LevelLayer *layer, Tile tile)
{
    vector_add(&layer->tiles, tile);
}

void level_layer_draw(const LevelLayer *layer, SDL_Renderer *renderer)
{
    for (size_t i = 0; i < vector_size(layer->tiles); i++)
    {
        tile_draw(&layer->tiles[i], renderer);
    }
}
