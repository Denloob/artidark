#pragma once

#include "SDL.h"
#include "hashmap.h"
#include <stdbool.h>

typedef enum TileCallbackType
{
    TILE_CALLBACK_NONE,
    TILE_CALLBACK_DOOR
} TileCallbackType;

struct TileCallbackDoorArgument
{
    TileCallbackType type;  // TILE_CALLBACK_DOOR
    char *destinationLevel; // The name of the level where this door leads too.
};

typedef union TileArguments
{
    TileCallbackType type;
    struct TileCallbackDoorArgument door;
} TileArguments;

typedef void (*TileCallback)(TileArguments *args);

typedef struct TileCallbackInfo
{
    char *name;
    TileCallbackType type;
    TileCallback callback;
} TileCallbackInfo;

typedef struct Tile
{
    SDL_FRect hitbox;
    SDL_Texture *texture;
    bool solid;
    TileCallback callback;
    TileArguments *args;
} Tile;

typedef Tile *VecTile;

/**
 * @brief Initializes the tile callbacks
 * @see tile_callback_cleanup
 */
void tile_callback_init();

/**
 * @brief Cleans up the memory used by the tile callbacks
 */
void tile_callback_cleanup();

/**
 * @brief Calls cleanup function for the matching callback type.
 *
 * @param args The callback arguments to be cleaned up.
 */
void tile_callback_args_cleanup(TileArguments *args);

/**
 * @brief Gets the tile callback function with the given name.
 *
 * @param name The name of the callback.
 * @return The callback function and it's type.
 * @warning Do not free the returned pointer.
 *
 * @warning Always initialize the tile callbacks before using.
 * @see tile_callback_init
 * @see tile_callback_cleanup
 */
const TileCallbackInfo *tile_callback_get(const char *name);

/**
 * @brief Called when a door event is triggered
 *
 * @param args Union with struct DoorArgument set.
 *
 * @see tile_callback_get
 */
void tile_callback_door(TileArguments *args);

/**
 * @brief Does nothing
 */
void tile_callback_none(TileArguments *);

/**
 * @brief Cleans up memory used by the door callback arguments.
 *
 * @param args The callback arguments to clean up.
 */
void tile_callback_door_cleanup(struct TileCallbackDoorArgument *args);

/**
 * @brief Initializes a tile
 *
 * @param tile The tile to initialize.
 * @param hitbox The hitbox of the tile. If width or height are 0, they are
 *                  determined by the texture.
 * @param texture The texture of the tile.
 * @param solid Whether the tile is solid or not.
 * @param callback The callback function for the tile.
 * @param args The callback arguments for the tile (managed by the caller).
 */
void tile_init(Tile *tile, SDL_FRect hitbox, SDL_Texture *texture, bool solid,
               TileCallback callback, TileArguments *args);

// There is no clean-up as there is no memory managed by the tile.

/**
 * @brief Draws the tile.
 *
 * @param renderer The renderer to draw onto.
 * @param offset The offset to apply to the tile's x and y.
 *
 * @see renderer_renderCopyWithOffsetF
 */
void tile_draw(const Tile *tile, SDL_Renderer *renderer, SDL_FPoint *offset);
