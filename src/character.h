#pragma once

#include "SDL.h"

typedef struct Character
{
    SDL_FRect hitbox;
    SDL_FPoint velocity;
} Character;

/**
 * @brief Draws the character.
 *
 * @param renderer The renderer to draw onto.
 */
void character_draw(const Character *character, SDL_Renderer *renderer);

/**
 * @brief Updates character's fields.
 */
void character_tick(Character *character);

/**
 * @brief Applies the character's velocity to it's position.
 */
void character_applyVelocity(Character *character);

/**
 * @brief Applies gravity to the character's velocity.
 *
 * @param gravity The acceleration of gravity.
 */
void character_applyGravity(Character *character, float gravity);
