#pragma once

#include "SDL.h"

/** Initializes SDL, SDL_Window and SDL_Renderer.
 * @param window Pointer to the window to initialize
 * @param renderer Pointer to the renderer to initialize
 * @param windowName The name of the window to initialize
 * @param width The width of the window
 * @param height The height of the window
 *
 * @return EXIT_SUCCESS on success or EXIT_FAILURE on fail
 */
int initSDL(SDL_Window **window, SDL_Renderer **renderer,
            const char *windowName, int width, int height);
/** Tries to initialize the window. If can't, calls SDL_Quit and printf the SDL error.
 * @param window Pointer to the window to initialize
 * @param windowName The name of the window to initialize
 * @param width The width of the window
 * @param height The height of the window
 *
 * @return EXIT_SUCCESS on success or EXIT_FAILURE on fail
 */
int initWindow(SDL_Window **window, const char *windowName, int width,
               int height);

/** Tries to initialize the renderer. If can't, destroys the window, calls SDL_Quit and printf the SDL error.
 * @param window Pointer to the window to initialize
 * @param renderer Pointer to the renderer to initialize
 *
 * @return EXIT_SUCCESS on success or EXIT_FAILURE on fail
 */
int initRenderer(SDL_Renderer **renderer, SDL_Window **window);

/** Wrapper for the SDL_RenderCopyF function, offsets the dstrect position.
 *
 * @param offset The offset to apply to the dstrect x and y.
 *                  When NULL, no offset is applied.
 *
 * @see SDL_RenderCopyF
 */
int renderer_renderCopyWithOffsetF(SDL_Renderer *renderer, SDL_Texture *texture,
                                   const SDL_Rect *srcrect,
                                   const SDL_FRect *dstrect,
                                   SDL_FPoint *offset);
