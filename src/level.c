#include "SDL.h"
#include "level.h"
#include "tile.h"
#include "utils.h"
#include "vec.h"

Level *level_create(void)
{
    Level *level = xmalloc(sizeof(*level));

    level->tiles = vector_create();

    return level;
}

void level_destroy(Level *level)
{
    vector_free(level->tiles);
    free(level);
}

void level_draw(const Level *level, SDL_Renderer *renderer)
{
    for (size_t i = 0; i < vector_size(level->tiles); i++)
    {
        tile_draw(&level->tiles[i], renderer);
    }
}

void level_add_tile(Level *level, Tile tile)
{
    vector_add(&level->tiles, tile);
}
