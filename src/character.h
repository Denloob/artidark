#pragma once

#include "SDL.h"
#include "tile.h"
#include "vec.h"

typedef struct Character
{
    SDL_FRect hitbox;
    SDL_FPoint velocity;
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

/**
 * @brief Draws the character.
 *
 * @param renderer The renderer to draw onto.
 */
void character_draw(const Character *character, SDL_Renderer *renderer);

/**
 * @brief Updates character's fields.
 *
 * @param tiles The tiles surrounding the character.
 */
void character_tick(Character *character, const VecTile tiles);

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
