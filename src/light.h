#pragma once

#include "SDL.h"

typedef struct Light
{
    SDL_Point pos; // The center of the light.

    SDL_Color color_rgb; /* The color of the light.
                            Modification requires spot_texture re-render */
    int radius;          /* The radius of light's reach.
                          * Modification requires spot_texture re-render */

    /* const pointer: Do not reassign. */
    SDL_Renderer *renderer;

    /* private: Do not read, do not write */
    SDL_Texture *spot_texture;
} Light;

typedef struct LightLayer
{
    SDL_Texture *texture;

    /* const pointer: Do not reassign. */
    SDL_Renderer *renderer;
} LightLayer;

/**
 * @brief Creates a LightLayer.
 *
 * @param renderer The renderer which will be used for the layer.
 * @param darkness_color_rgb The RGB color to be used for the darkness.
 * @return The created LightLayer.
 */
LightLayer *light_layer_create(SDL_Renderer *renderer,
                               SDL_Color darkness_color_rgb);

/**
 * @brief Destroys the LightLayer.
 */
void light_layer_destroy(LightLayer *layer);

/**
 * @brief Draws the given Light onto the given LightLayer.
 *
 * @param light The Light to draw.
 */
void light_layer_add(LightLayer *layer, const Light *light);

/**
 * @brief Draws the given LightLayer onto the renderer that was used at creation.
 */
void light_layer_draw(LightLayer *layer);

/**
 * @brief Creates a Light with the given properties.
 *
 * @param renderer The renderer which will be used for the texture.
 * @param pos The position of the light source in the game
 *              (Will be the center of the light spot).
 * @param radius The radius of the light spot.
 * @param color_rgb The RGB color of the light.
 * @return The created Light.
 *
 * @see light_destroy
 */
Light *light_create(SDL_Renderer *renderer, SDL_Point pos, int radius,
                    SDL_Color color_rgb);

/**
 * @brief Destroys the light and the spot texture.
 *
 * @see light_create
 * @see light_get_light_spot
 */
void light_destroy(Light *light);

/**
 * @brief Re-renders the spot texture of the light.
 */
void light_re_render(Light *light);

/**
 * @brief Draws the light such that the pos is the center of it.
 *          Uses the renderer used at creation.
 *
 * @see light_create_source
 */
void light_draw(const Light *light);
