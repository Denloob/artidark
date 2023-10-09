#include "SDL.h"
#include "light.h"
#include "utils.h"

SDL_Texture *light_create_light_spot(const Light *light)
{
    // TODO: Check if using SDL_Surface pixels is faster
    SDL_Renderer *renderer = light->renderer;
    const int radius = light->radius;

    SDL_Texture *prev_renderer_target = SDL_GetRenderTarget(renderer);

    const int diameter = radius * 2;

    SDL_Texture *circle_texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_TARGET, diameter, diameter);
    SDL_SetTextureBlendMode(circle_texture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, circle_texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    for (int y = 0; y < diameter; y++)
    {
        for (int x = 0; x < diameter; x++)
        {
            const int x_d = x - radius;
            const int y_d = y - radius;
            const double distance = SDL_sqrt(x_d * x_d + y_d * y_d);

            if (distance > radius)
                continue;

            const Uint8 alpha = 255 * (1.0 - distance / radius);
            const SDL_Color rgb = light->color_rgb;
            SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, alpha);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    SDL_SetRenderTarget(renderer, prev_renderer_target);

    return circle_texture;
}

Light *light_create(SDL_Renderer *renderer, SDL_Point pos, int radius,
                    SDL_Color color_rgb)
{
    Light *light = xmalloc(sizeof(*light));

    light->pos = pos;
    light->radius = radius;
    light->color_rgb = color_rgb;

    light->renderer = renderer;

    light->spot_texture = NULL;

    light_re_render(light);

    return light;
}

void light_destroy(Light *light)
{
    free(light);
}

void light_draw(const Light *light)
{
    const int radius = light->radius;
    const int diameter = radius * 2;

    SDL_Texture *spot = light->spot_texture;

    // Center the light spot so pos is the center of it.
    const SDL_Rect dstrect = {
        light->pos.x - radius,
        light->pos.y - radius,
        diameter,
        diameter,
    };

    SDL_RenderCopy(light->renderer, spot, NULL, &dstrect);
}

void light_re_render(Light *light)
{
    if (light->spot_texture)
        SDL_DestroyTexture(light->spot_texture);

    light->spot_texture = light_create_light_spot(light);
}

LightLayer *light_layer_create(SDL_Renderer *renderer,
                               SDL_Color darkness_color_rgb)
{
    LightLayer *layer = xmalloc(sizeof(*layer));

    layer->renderer = renderer;

    int w, h;
    SDL_Window *window = SDL_RenderGetWindow(renderer);
    SDL_GetWindowSize(window, &w, &h);

    layer->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_TARGET, w, h);

    {
        SDL_Texture *prev_renderer_target = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, layer->texture);

        SDL_Color c = darkness_color_rgb;

        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 0xff);
        SDL_RenderClear(renderer);

        SDL_SetRenderTarget(renderer, prev_renderer_target);
    }

    SDL_SetTextureBlendMode(layer->texture, SDL_BLENDMODE_MOD);

    return layer;
}

void light_layer_destroy(LightLayer *layer)
{
    SDL_DestroyTexture(layer->texture);
    free(layer);
}

void light_layer_add(LightLayer *layer, const Light *light)
{
    SDL_Texture *prev_renderer_target = SDL_GetRenderTarget(light->renderer);
    SDL_SetRenderTarget(light->renderer, layer->texture);

    light_draw(light);

    SDL_SetRenderTarget(light->renderer, prev_renderer_target);
}

void light_layer_draw(LightLayer *layer)
{
    SDL_Window *window = SDL_RenderGetWindow(layer->renderer);
    SDL_Rect window_rect = {.x = 0, .y = 0};
    SDL_GetWindowSize(window, &window_rect.w, &window_rect.h);

    SDL_RenderCopy(layer->renderer, layer->texture, NULL, &window_rect);
}
