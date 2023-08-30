#pragma once

#include "SDL.h"
#include "tile.h"

#define DIR_SEPARATOR '/'

typedef struct TilesetEntry
{
    bool solid;
    int id;
    SDL_Texture *texture;
    TileCallback callback;
    TileArguments args;
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
 * @param textureDirPath The path to the texture directory. (Managed by the tileset)
 * @see Tileset
 * @param renderer The renderer to use to load the textures.
 * @return The created tileset.
 *
 * @see tileset_destroy
 */
Tileset *tileset_load(FILE *stream, char *textureDirPath,
                      SDL_Renderer *renderer);

/**
 * @brief Creates a tileset
 *
 * @param textureDirPath The path to the texture directory. (Managed by the tileset)
 * @see Tileset
 * @return The created tileset.
 *
 * @warning This function relies on tile_callback_get, this means that the tile
 *              callback must be initialized before this function is called.
 * @see tile_callback_get
 * @see tile_callback_init
 *
 * @see tileset_destroy
 */
Tileset *tileset_create(char *textureDirPath);

/**
 * @brief Destroys a tileset.
 * @warning Destroys the textures, frees the textureDirPath and the
 *           TileArguments.
 */
void tileset_destroy(Tileset *tileset);

/**
 * @brief Finds tile texture in a tileset using it's id.
 *          If the out parameter is NULL, nothing will be written to it.
 *
 * @param id The id of the tile to find.
 * @param[out] texture The texture of the tile.
 * @param[out] solid Whether the tile is solid or not.
 * @param[out] callback The callback function for the tile.
 * @param[out] args The callback arguments for the tile.
 * @return EXIT_SUCCESS if id exists, EXIT_FAILURE otherwise.
 */
int tileset_QueryTextureByID(const Tileset *tileset, int id,
                             SDL_Texture **texture, bool *solid,
                             TileCallback *callback, TileArguments **args);
