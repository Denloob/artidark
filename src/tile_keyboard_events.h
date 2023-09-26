#pragma once

#include "SDL.h"
#include "hashmap.h"
#include "tile_callback.h"
#include "tileset.h"
#include "utils.h"

/**
 * @brief Convenience macro to subscribe a tile type (textureID) from a
 *          tileset to a key.
 *
 * @param subscribers (KeyEventSubscribers *)
 * @param key (SDL_Keycode) The event keycode to subscribe to.
 * @param tileset (const Tileset *) The tileset from which to query the TileCallback.
 * @param textureID (int) The textureID for which to look up the TileCallback.
 *
 * @see tile_keyboard_events_subscribe_dynamic
 * @see tile_keyboard_events_unsubscribe
 */
#define tile_keyboard_events_subscribe(subscribers, key, tileset, textureID)   \
    do                                                                         \
    {                                                                          \
        TileCallback *tileCallback = xmalloc(sizeof(*tileCallback));           \
        tileset_QueryTextureByID(tileset, textureID, NULL, NULL,               \
                                 tileCallback);                                \
        tile_keyboard_events_subscribe_dynamic(subscribers, key,               \
                                               tileCallback);                  \
    } while (0)

typedef TileCallback **vec_TileCallback;

typedef HASHMAP(SDL_Keycode, TileCallback *) KeyEventSubscribers;

/**
 * @brief Creates a new map for keyboard events subscribers.
 *
 * @return A new map for keyboard events subscribers.
 *
 * @see tile_keyboard_events_destroy
 */
KeyEventSubscribers *tile_keyboard_events_create();

/**
 * @brief Destroys a map for keyboard events subscribers and all the
 *          subscribers. Also frees all the subscribed TileCallbacks (doesn't
 *          cleanup, only free).
 *
 * @see tile_keyboard_events_create
 */
void tile_keyboard_events_destroy(KeyEventSubscribers *subscribers);

/**
 * @brief Subscribe to keyboard events.
 *
 * @param key The event keycode to subscribe to.
 * @param callback Dynamically allocated struct of the subscriber's callback
 *                  (managed by the subscriber list until unsubscribed.
 *                      Will be freed at destruction).
 *
 * @see tile_keyboard_events_unsubscribe
 * @see tile_keyboard_events_destroy
 */
void tile_keyboard_events_subscribe_dynamic(KeyEventSubscribers *subscribers,
                                            SDL_Keycode key,
                                            TileCallback *callback);

/**
 * @brief Unsubscribe from keyboard events.
 *
 * @param key The event keycode to unsubscribe from.
 * @param callback The callback of the subscriber (the exact same pointer used
 *                  for subscription).
 *
 * @see tile_keyboard_events_subscribe
 */
void tile_keyboard_events_unsubscribe(KeyEventSubscribers *subscribers,
                                      SDL_Keycode key, TileCallback *callback);

/**
 * @brief Notifies (calls) all the subscribers for a given key with their args.
 *
 * @param key The event keycode subscribers of which to notify.
 * @param game_state The current state of the game to be used by the callback.
 *                      The key will be auto set by the key parameter.
 *                      The id will be also auto set.
 */
void tile_keyboard_events_notify(KeyEventSubscribers *subscribers,
                                 SDL_Keycode key,
                                 CallbackGameState *game_state);

/**
 * @brief Loads keyboard mappings from file stream.
 *
 * @param stream The file stream to load from.
 * @param tileset The tileset which will be used for loading the keymap
 * @return A new map for keyboard events subscribers.
 *
 * @see tile_keyboard_events_destroy
 */
KeyEventSubscribers *tile_keyboard_mappings_load(FILE *stream,
                                                 Tileset *tileset);
