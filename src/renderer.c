#include "SDL.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

int renderer_render_copy_with_offset_f(SDL_Renderer *renderer,
                                       SDL_Texture *texture,
                                       const SDL_Rect *srcrect,
                                       const SDL_FRect *dstrect,
                                       SDL_FPoint *offset)
{
    const float x_offset = offset ? offset->x : 0;
    const float y_offset = offset ? offset->y : 0;
    const SDL_FRect dstrect_with_offset = {
        .x = dstrect->x + x_offset,
        .y = dstrect->y + y_offset,
        .w = dstrect->w,
        .h = dstrect->h,
    };

    return SDL_RenderCopyF(renderer, texture, srcrect, &dstrect_with_offset);
}
