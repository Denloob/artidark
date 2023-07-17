#pragma once

#include "SDL.h"
#include "tile.h"
#include "vec.h"

typedef struct Character
{
    SDL_FRect hitbox;
    SDL_FPoint velocity;
    SDL_Texture *texture;
} Character;

/**
 * @brief Fixes character clipping through tile on one axis.
 *
 * @param tiles Tiles the character could be possibly clipped through.
 * @param posAxis `x` or `y`
 * @param sizeAxis `w` or `h`
 */
#define character_fixTileClip(character, tiles, posAxis, sizeAxis)             \
    {                                                                          \
        VecTile *collisions = character_findCollisions(character, tiles);      \
        float direction = (character->velocity).posAxis;                       \
                                                                               \
        for (size_t i = 0; i < vector_size(collisions); i++)                   \
            if (direction < 0)                                                 \
                (character->hitbox).posAxis =                                  \
                    (collisions[i]->hitbox).posAxis +                          \
                    (collisions[i]->hitbox).sizeAxis;                          \
            else if (direction > 0)                                            \
                (character->hitbox).posAxis =                                  \
                    (collisions[i]->hitbox).posAxis -                          \
                    (character->hitbox).sizeAxis;                              \
                                                                               \
        vector_free(collisions);                                               \
    }

/** Clamps the velocity of character on given axis between -max_velocity and
 *      +max_velocity.
 */
#define character_clampVelocityOnAxis(character, axis, max_velocity)           \
    {                                                                          \
        (character->velocity).axis = SDL_clamp((character->velocity).axis,     \
                                               -max_velocity, max_velocity);   \
    }

/**
 * @brief Creates a character
 *
 * @param texture The texture of the character.
 * @warning The caller is responsible for managing the texture.
 *
 * @param hitbox The hitbox of the character. If width or height is 0, they are
 *                  determined by the texture. Scaling will be applied after
 *                  calculations.
 *               When drawing, the texture will be stretched to fit the hitbox.
 * @param scalingFactor The scaling factor to apply to the hitbox width and
 *                      height.
 * @return The created character
 */
Character *character_create(SDL_Texture *texture, SDL_FRect hitbox, int scalingFactor);

/**
 * @brief Destroys the character
 * @warning Doesn't destroy the texture.
 */
void character_destroy(Character *character);

/**
 * @brief Clamps the velocity of the character between -max_velocity and
 *          +max_velocity.
 *
 * @param max_velocity The maximum velocity the character can have in any
 *                      direction.
 */
void character_clamp_velocity(Character *character, float max_velocity);

/**
 * @brief Draws the character.
 *
 * @param renderer The renderer to draw onto.
 */
void character_draw(const Character *character, SDL_Renderer *renderer);

/**
 * @brief Updates character's fields and ensures they all are valid.
 *
 * @param tiles The tiles surrounding the character.
 * @param max_acceleration The maximum acceleration the character can have.
 */
void character_tick(Character *character, const VecTile tiles,
                    float max_acceleration);

/**
 * @brief Moves the character as needed and checks that the character doesn't
 *          clip through tiles.
 *
 * @param tiles The tiles surrounding the character.
 */
void character_move(Character *character, const VecTile tiles);

/**
 * @brief Applies gravity to the character's velocity.
 *
 * @param gravity The acceleration of gravity.
 */
void character_applyGravity(Character *character, float gravity);

/**
 * @brief Finds the collisions between the character and the tiles.
 *
 * @param tiles The tiles to find collisions with.
 * @return Vector of pointers to all the tiles player collides with.
 */
VecTile *character_findCollisions(const Character *character,
                                  const VecTile tiles);
