#pragma once

#include "SDL_keycode.h"

typedef enum TileCallbackType
{
    TILE_CALLBACK_NONE,
    TILE_CALLBACK_LADDER,
} TileCallbackType;

struct TileCallbackLadderArgument
{
    TileCallbackType type; // TILE_CALLBACK_LADDER
};

typedef union TileArguments
{
    TileCallbackType type;
    struct TileCallbackLadderArgument ladder;
} TileArguments;

typedef struct CallbackGameState
{
    struct Character *character;
    struct LevelHashmap *levels;
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
 * @brief Called when a ladder event is triggered
 *
 * @param args Union with struct LadderArgument set.
 * @param game_state The current state of the game.
 *
 * @see tile_callback_get
 */
void tile_callback_ladder(TileArguments *args, CallbackGameState *game_state);

/**
 * @brief Does nothing
 */
void tile_callback_none(TileArguments *, CallbackGameState *);
