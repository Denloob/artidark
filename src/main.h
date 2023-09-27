#pragma once

#include "character.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Minimal number of pixels to keep between the player and the screen border.
#define CHARACTER_SCROLL_BORDER_HORIZONTAL (WINDOW_WIDTH / 4.0)
#define CHARACTER_SCROLL_BORDER_VERTICAL (WINDOW_HEIGHT / 3.0)

#define WINDOW_NAME "Game"

#define BACKGROUND_COLOR 0x00, 0x00, 0x00, 0xFF

#define FPS 60

#define FRAME_DURATION (1000 / FPS)

#define GRAVITY 0.9

#define SCALING_FACTOR 2

#define TILE_SIZE 16

// NOTE: should not be larger than the smallest solid tile size.
// Otherwise the character will be able to clip trough it.
#define MAX_ACCELERATION (TILE_SIZE)

#define CHARACTER_SPEED 3
#define CHARACTER_JUMP_STRENGTH 30

/**
 * @brief Calculates the offset for rendering so that the character stays on
 *          screen even if it's x/y pos is out of window's borders.
 *
 * @param character The character for which to calculate the offset.
 * @param previous_offset The previous offset for the renderer.
 * @param new_offset[out] The offset for the renderer.
 *
 * @see renderer_render_copy_with_offset_f
 */
void calculate_rendering_offset(const Character *character,
                                SDL_FPoint previous_offset,
                                SDL_FPoint *new_offset);

/**
 * @brief Initializes the given window and renderer with default parameters,
 *          with WINDOW_{WIDTH,HEIGHT,NAME}, in the center and with VSync
 *          enabled.
 *
 * @param window_ptr Pointer to the SDL window to initialize
 * @param renderer_ptr Pointer to the SDL renderer to initialize
 *
 * @see quit_sdl
 */
void init_sdl(SDL_Window **window_ptr, SDL_Renderer **renderer_ptr);

/**
 * @brief Destroys the given window and renderer and quits SDL.
 *
 * @param window The SDL window to destroy
 * @param renderer The SDL renderer to destroy
 *
 * @see init_sdl
 */
void quit_sdl(SDL_Window *window, SDL_Renderer *renderer);
