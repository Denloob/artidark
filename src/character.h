#pragma once

#include "SDL.h"
#include "level_layer.h"
#include "tile.h"
#include "vec.h"
#include <stdint.h>

typedef uint8_t CharacterMovementDirection;

#define CHARACTER_MOVE_LEFT (CharacterMovementDirection)0x01  // 0b01
#define CHARACTER_MOVE_RIGHT (CharacterMovementDirection)0x02 // 0b10

typedef uint8_t CharacterCollisionDirection;

#define CHARACTER_COLLISION_TOP (CharacterCollisionDirection)0x01
#define CHARACTER_COLLISION_BOTTOM (CharacterCollisionDirection)0x02
#define CHARACTER_COLLISION_LEFT (CharacterCollisionDirection)0x04
#define CHARACTER_COLLISION_RIGHT (CharacterCollisionDirection)0x08

typedef struct Character
{
    SDL_FRect hitbox;
    SDL_FPoint velocity;
    SDL_Texture *texture;
    int speed;
    int jumpStrength;
    CharacterMovementDirection movementDirection;
    CharacterCollisionDirection collisions;
} Character;

typedef void (*MovementFunction)(Character *, CharacterMovementDirection);

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
 * @param jumpStrength The jump strength of the character.
 * @param scalingFactor The scaling factor to apply to the hitbox width and
 *                      height.
 * @return The created character
 */
Character *character_create(SDL_Texture *texture, SDL_FRect hitbox, int speed,
                            int jumpStrength, int scalingFactor);

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
 * @param layers Layers vector which contain tiles with which the player could
 *                  have collided.
 * @param max_acceleration The maximum acceleration the character can have.
 */
void character_tick(Character *character, const VecLevelLayer layers,
                    float max_acceleration);

/**
 * @brief Moves the character as needed and checks that the character doesn't
 *          clip through tiles.
 *
 * @param layers Layers vector which contain tiles with which the player could
 *                  have collided.
 */
void character_tickMovement(Character *character, const VecLevelLayer layers);

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
 * @brief Sets the character's collisions to the given collision, while keeping
 *          the previous collision data.
 *
 * @param The collision direction to set.
 * @see CharacterCollisionDirection
 */
void character_setCollision(Character *character,
                            CharacterCollisionDirection direction);

/**
 * @brief Un-sets the character's collisions to the given direction.
 *
 * @param The collision direction to unset.
 */
void character_unsetCollision(Character *character,
                              CharacterCollisionDirection direction);

/**
 * @brief Checks if the character is currently standing (touching) the ground.
 *
 * @return True if yes, false otherwise.
 */
bool character_isOnGround(Character *character);

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

/**
 * @brief Finds the collisions between the character and the tiles in layers.
 *
 * @param layers Layers vector which contain tiles to find collision with.
 * @return Vector of pointers to all the tiles player collides with.
 * @see character_findCollisions
 */
VecTile *character_findCollisionsWithLayerTiles(const Character *character,
                                                const VecLevelLayer layers);
