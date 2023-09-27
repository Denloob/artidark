#pragma once

#include "SDL.h"

/** Wrapper for the SDL_RenderCopyF function, offsets the dstrect position.
 *
 * @param offset The offset to apply to the dstrect x and y.
 *                  When NULL, no offset is applied.
 *
 * @see SDL_RenderCopyF
 */
int renderer_render_copy_with_offset_f(SDL_Renderer *renderer,
                                       SDL_Texture *texture,
                                       const SDL_Rect *srcrect,
                                       const SDL_FRect *dstrect,
                                       SDL_FPoint *offset);
