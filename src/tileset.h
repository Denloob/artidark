#pragma once

#include "SDL.h"

#define DIR_SEPARATOR '/'

typedef struct TilesetEntry
{
    bool solid;
    int id;
    SDL_Texture *texture;
} TilesetEntry;

typedef TilesetEntry *VecTilesetEntry;

typedef struct Tileset
{
    VecTilesetEntry entries;
    char *textureDirPath; // Path which will be used to find path to
                          // textures in the csv
} Tileset;

/**
 * @brief Initializes a tileset entry
 *
 * @param id ID of the entry
 * @param texture The texture of the tile represented by the ID.
 * @warning The texture will not be copied, but instead just stored in the
 *          entry. So don't destroy it.
 */
void tileset_entry_init(TilesetEntry *entry, int id, SDL_Texture *texture);

/**
 * @brief Cleans up memory used by a tileset entry.
 *
 * @warning Frees the texture.
 */
void tileset_entry_cleanup(TilesetEntry *entry);

/**
 * @brief Creates a tileset from a csv stream.
 *
 * @param stream The csv stream where each row is `id,texturePath`.
 * @param textureDirPath The path to the texture directory.
 * @see Tileset
 * @param renderer The renderer to use to load the textures.
 * @return The created tileset.
 */
Tileset *tileset_load(FILE *stream, char *textureDirPath,
                      SDL_Renderer *renderer);

/**
 * @brief Creates a tileset
 *
 * @param textureDirPath The path to the texture directory
 * @see Tileset
 * @return The created tileset.
 */
Tileset *tileset_create(char *textureDirPath);

/**
 * @brief Destroys a tileset.
 */
void tileset_destroy(Tileset *tileset);

/**
 * @brief Finds tile texture in a tileset using it's id.
 *          If the out parameter is NULL, nothing will be written to it.
 *
 * @param id The id of the tile to find.
 * @param[out] texture The texture of the tile.
 * @param[out] solid Whether the tile is solid or not.
 * @return EXIT_SUCCESS if id exists, EXIT_FAILURE otherwise.
 */
int tileset_QueryTextureByID(const Tileset *tileset, int id,
                             SDL_Texture **texture, bool *solid);
