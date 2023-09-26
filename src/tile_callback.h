#pragma once

#include "SDL_keycode.h"
typedef enum TileCallbackType
{
    TILE_CALLBACK_NONE,
    TILE_CALLBACK_DOOR
} TileCallbackType;

struct TileCallbackDoorArgument
{
    TileCallbackType type;   // TILE_CALLBACK_DOOR
    char *destination_level; // The name of the level where this door leads too.
};

typedef union TileArguments
{
    TileCallbackType type;
    struct TileCallbackDoorArgument door;
} TileArguments;

typedef struct CallbackGameState
{
    struct Character *character;
    struct Level **level_ptr;
    SDL_Keycode key;     // Key that triggered the call
    int tile_texture_id; // The id of the tile for the event
} CallbackGameState;

typedef void (*TileCallbackFunction)(TileArguments *args,
                                     CallbackGameState *game_info);

typedef struct TileCallbackInfo
{
    char *name;
    TileCallbackType type;
    TileCallbackFunction callback;
} TileCallbackInfo;

typedef struct TileCallback
{
    TileCallbackFunction func;
    TileArguments *args;
    int id; // The id of the tile
} TileCallback;

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
 * @param game_state The current state of the game.
 *
 * @see tile_callback_get
 */
void tile_callback_door(TileArguments *args, CallbackGameState *game_state);

/**
 * @brief Does nothing
 */
void tile_callback_none(TileArguments *, CallbackGameState *);

/**
 * @brief Cleans up memory used by the door callback arguments.
 *
 * @param args The callback arguments to clean up.
 */
void tile_callback_door_cleanup(struct TileCallbackDoorArgument *args);
