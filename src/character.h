#pragma once

#include "SDL.h"
#include "tile.h"
#include "vec.h"
#include <stdint.h>

typedef uint8_t CharacterMovementDirection;

#define CHARACTER_MOVE_LEFT (CharacterMovementDirection)0x01  // 0b01
#define CHARACTER_MOVE_RIGHT (CharacterMovementDirection)0x02 // 0b10

typedef struct Character
{
    SDL_FRect hitbox;
    SDL_FPoint velocity;
    SDL_Texture *texture;
    int speed;
    CharacterMovementDirection movementDirection;
} Character;

typedef void (*MovementFunction)(Character *, CharacterMovementDirection);

/**
 * @brief Fixes character clipping through tile on one axis.
 *
 * @param tiles Tiles the character could be possibly clipped through.
 * @param movementDelta Delta of player's movement.
 * @param posAxis `x` or `y`
 * @param sizeAxis `w` or `h`
 */
#define character_fixTileClip(character, tiles, movementDelta, posAxis,        \
                              sizeAxis)                                        \
    {                                                                          \
        VecTile *collisions = character_findCollisions(character, tiles);      \
                                                                               \
        for (size_t i = 0; i < vector_size(collisions); i++)                   \
        {                                                                      \
            if (movementDelta < 0)                                             \
                (character->hitbox).posAxis =                                  \
                    (collisions[i]->hitbox).posAxis +                          \
                    (collisions[i]->hitbox).sizeAxis;                          \
            else if (movementDelta > 0)                                        \
                (character->hitbox).posAxis =                                  \
                    (collisions[i]->hitbox).posAxis -                          \
                    (character->hitbox).sizeAxis;                              \
            else                                                               \
                continue;                                                      \
                                                                               \
            (character->velocity).posAxis = 0;                                 \
        }                                                                      \
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
 * @param speed The speed of the character.
 * @param scalingFactor The scaling factor to apply to the hitbox width and
 *                      height.
 * @return The created character
 */
Character *character_create(SDL_Texture *texture, SDL_FRect hitbox, int speed,
                            int scalingFactor);

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
void character_tickMovement(Character *character, const VecTile tiles);

/**
 * @brief Sets character's movement to the given direction, while keeping the
 *          previous movement data.
 *
 * @param direction The direction to move to.
 * @see CharacterMoveDirection
 */
void character_setMovement(Character *character,
                           CharacterMovementDirection direction);

/**
 * @brief Un-sets character's movement to the given direction.
 *
 * @param direction The direction to unset.
 */
void character_unsetMovement(Character *character,
                             CharacterMovementDirection direction);

/**
 * @brief Handles keyboard events (KEYDOWN / KEYUP) related to the character.
 *
 * @param event The keyboard event.
 */
void character_handleKeyboardEvent(Character *character,
                                   SDL_KeyboardEvent *event);

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
