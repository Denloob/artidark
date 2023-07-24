#pragma once

#include "SDL.h"
#include "level_layer.h"
#include "tileset.h"

#define LEVEL_LAYER_SEPARATOR '\\'

typedef struct Level
{
    char *name;
    VecLevelLayer layers;
} Level;

/**
 * @brief Creates a level.
 *
 * @param name The name of the level. Managed by the level.
 * @warning The name will be freed by the destructor.
 * @return The created level.
 */
Level *level_create(char *name);

/**
 * @brief Destroys the level.
 *
 * @param level The level to destroy.
 */
void level_destroy(Level *level);

/**
 * @brief Adds a layer to the level.
 *
 * @param layer The layer to add.
 */
void level_addLayer(Level *level, LevelLayer *layer);

/**
 * @brief Draws the level.
 *
 * @param renderer The renderer to draw onto.
 */
void level_draw(const Level *level, SDL_Renderer *renderer);

/**
 * @brief Loads a level from a file.
 *
 * @param stream The stream to load level from. First line is expected to be
 *                  the name of the level.
 * @param tileset The tileset to use for the textures.
 * @param tileWidth The width of a tile in the level (before scaling).
 * @param tileHeight The height of a tile in the level (before scaling).
 * @param scalingFactor The scaling factor to apply to each tile.
 * @return The loaded level.
 */
Level *level_load(FILE *stream, const Tileset *tileset, int tileWidth,
                  int tileHeight, int scalingFactor);
