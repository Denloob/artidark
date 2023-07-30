#include "SDL.h"
#include "character.h"
#include "level_layer.h"
#include "renderer.h"
#include "tile.h"
#include "utils.h"
#include "vec.h"
#include <stddef.h>

#define CHARACTER_COLLISION_y_NEGATIVE CHARACTER_COLLISION_TOP
#define CHARACTER_COLLISION_y_POSITIVE CHARACTER_COLLISION_BOTTOM
#define CHARACTER_COLLISION_x_NEGATIVE CHARACTER_COLLISION_LEFT
#define CHARACTER_COLLISION_x_POSITIVE CHARACTER_COLLISION_RIGHT

/**
 * @brief Applies collision physics to the character.
 *
 * @param collisions The collisions between the character and tiles they could
 *                      they could collided with.
 * @param movementDelta Delta of player's movement.
 * @param posAxis `x` or `y`
 * @param sizeAxis `w` or `h`
 */
#define character_applyCollisionsAfterMovement(                                \
    character, collisions, movementDelta, posAxis, sizeAxis)                   \
    {                                                                          \
        if (vector_size(collisions))                                           \
            (character->velocity).posAxis = 0;                                 \
        character_unsetCollision(                                              \
            character, (CHARACTER_COLLISION_##posAxis##_NEGATIVE) |            \
                           (CHARACTER_COLLISION_##posAxis##_POSITIVE));        \
        CharacterCollisionDirection collisionDirection = 0;                    \
                                                                               \
        for (size_t i = 0; i < vector_size(collisions); i++)                   \
        {                                                                      \
            if (movementDelta < 0)                                             \
            {                                                                  \
                (character->hitbox).posAxis =                                  \
                    (collisions[i]->hitbox).posAxis +                          \
                    (collisions[i]->hitbox).sizeAxis;                          \
                                                                               \
                collisionDirection |=                                          \
                    (CHARACTER_COLLISION_##posAxis##_NEGATIVE);                \
            }                                                                  \
            else if (movementDelta > 0)                                        \
            {                                                                  \
                (character->hitbox).posAxis =                                  \
                    (collisions[i]->hitbox).posAxis -                          \
                    (character->hitbox).sizeAxis;                              \
                                                                               \
                collisionDirection |=                                          \
                    (CHARACTER_COLLISION_##posAxis##_POSITIVE);                \
            }                                                                  \
        }                                                                      \
                                                                               \
        character_setCollision(character, collisionDirection);                 \
    }

/** Clamps the velocity of character on given axis between -max_velocity and
 *      +max_velocity.
 */
#define character_clampVelocityOnAxis(character, axis, max_velocity)           \
    {                                                                          \
        (character->velocity).axis = SDL_clamp((character->velocity).axis,     \
                                               -max_velocity, max_velocity);   \
    }

#define character_tickMovementOnAxis(character, layers, applyMovementFunc,     \
                                     posAxis, sizeAxis)                        \
    {                                                                          \
        float posBeforeMovement = (character)->hitbox.posAxis;                 \
        applyMovementFunc(character);                                          \
        float movementDelta = (character)->hitbox.posAxis - posBeforeMovement; \
                                                                               \
        VecTile *collisions =                                                  \
            character_findCollisionsWithLayerTiles(character, layers);         \
        character_applyCollisionsAfterMovement(                                \
            character, collisions, movementDelta, posAxis, sizeAxis);          \
        vector_free(collisions);                                               \
    }

Character *character_create(SDL_Texture *texture, SDL_FRect hitbox, int speed,
                            int jumpStrength, int scalingFactor)
{
    Character *character = xmalloc(sizeof(*character));

    character->texture = texture;

    if (hitbox.w == 0 || hitbox.h == 0)
    {
        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);

        if (hitbox.w == 0)
            hitbox.w = w;
        if (hitbox.h == 0)
            hitbox.h = h;
    }

    hitbox.w *= scalingFactor;
    hitbox.h *= scalingFactor;

    character->hitbox = hitbox;
    character->velocity = (SDL_FPoint){0, 0};
    character->movementDirection = 0;
    character->speed = speed;
    character->jumpStrength = jumpStrength;

    return character;
}

void character_destroy(Character *character)
{
    free(character);
}

void character_clamp_velocity(Character *character, float max_velocity)
{
    character_clampVelocityOnAxis(character, x, max_velocity);
    character_clampVelocityOnAxis(character, y, max_velocity);
}

void character_draw(const Character *character, SDL_Renderer *renderer,
                    SDL_FPoint *offset)
{
    renderer_renderCopyWithOffsetF(renderer, character->texture, NULL,
                                   &character->hitbox, offset);
}

SDL_FPoint character_getPosition(const Character *character)
{
    return (SDL_FPoint){character->hitbox.x, character->hitbox.y};
}

void character_tick(Character *character, const VecLevelLayer layers,
                    float max_acceleration)
{
    character_clamp_velocity(character, max_acceleration);

    character_tickMovement(character, layers);
}

/*
 * @brief Applies the horizontal movement the character, without checking for
 *          any collisions.
 * @see character_tickHorizontalMovement
 */
void character_applyHorizontalMovement(Character *character)
{
    character->hitbox.x += character->velocity.x;
    if (character->movementDirection & CHARACTER_MOVE_RIGHT)
        character->hitbox.x += character->speed;
    if (character->movementDirection & CHARACTER_MOVE_LEFT)
        character->hitbox.x -= character->speed;
}

/* @see character_tickMovement */
void character_tickHorizontalMovement(Character *character,
                                      const VecLevelLayer layers)
{
    character_tickMovementOnAxis(character, layers,
                                 character_applyHorizontalMovement, x, w);
}

/*
 * @brief Applies the vertical movement the character, without checking for
 *          any collisions.
 * @see character_tickVerticalMovement
 */
void character_applyVerticalMovement(Character *character)
{
    character->hitbox.y += character->velocity.y;
}

/* @see character_tickMovement */
void character_tickVerticalMovement(Character *character,
                                    const VecLevelLayer layers)
{
    character_tickMovementOnAxis(character, layers,
                                 character_applyVerticalMovement, y, h);
}

void character_tickMovement(Character *character, const VecLevelLayer layers)
{
    character_tickVerticalMovement(character, layers);
    character_tickHorizontalMovement(character, layers);
}

void character_setMovement(Character *character,
                           CharacterMovementDirection direction)
{
    character->movementDirection |= direction;
}

void character_unsetMovement(Character *character,
                             CharacterMovementDirection direction)
{
    character->movementDirection &= ~direction;
}

void character_setCollision(Character *character,
                            CharacterCollisionDirection direction)
{
    character->collisions |= direction;
}

void character_unsetCollision(Character *character,
                              CharacterCollisionDirection direction)
{
    character->collisions &= ~direction;
}

bool character_isOnGround(Character *character)
{
    return character->collisions & CHARACTER_COLLISION_BOTTOM;
}

/**
 * @brief Makes the character jump if they are on ground.
 */
void character_jump(Character *character)
{
    if (character_isOnGround(character))
        character->velocity.y -= character->jumpStrength;
}

void character_handleKeyboardEvent(Character *character,
                                   SDL_KeyboardEvent *event)
{
    SDL_Keycode keycode = event->keysym.sym;
    CharacterMovementDirection movementDirection = 0;
    switch (keycode)
    {
        case SDLK_SPACE:
            character_jump(character);
            return;
        case SDLK_d:
        case SDLK_RIGHT:
            movementDirection = CHARACTER_MOVE_RIGHT;
            break;
        case SDLK_a:
        case SDLK_LEFT:
            movementDirection = CHARACTER_MOVE_LEFT;
            break;
        default:
            return;
    }

    MovementFunction movementFunction = event->type == SDL_KEYDOWN
                                            ? character_setMovement
                                            : character_unsetMovement;

    movementFunction(character, movementDirection);
}

void character_applyGravity(Character *character, float gravity)
{
    character->velocity.y += gravity;
}

VecTile *character_findCollisions(const Character *character,
                                  const VecTile tiles)
{
    VecTile *collisions = vector_create();

    for (size_t i = 0; i < vector_size(tiles); i++)
    {
        if (tiles[i].solid &&
            SDL_HasIntersectionF(&character->hitbox, &tiles[i].hitbox))
        {
            // We need the no-lint because vector_add uses sizeof, and as our
            // vector is vector of pointers, so we do sizeof pointer, which is
            // marked as bugprone sizeof expression

            // NOLINTNEXTLINE(bugprone-sizeof-expression)
            vector_add(&collisions, &tiles[i]);
        }
    }

    return collisions;
}

VecTile *character_findCollisionsWithLayerTiles(const Character *character,
                                                const VecLevelLayer layers)
{
    VecTile *collisions = vector_create();
    for (size_t i = 0; i < vector_size(layers); i++)
    {
        VecTile *collisionsWithCurrentLayer =
            character_findCollisions(character, layers[i]->tiles);

        // NOLINTNEXTLINE(bugprone-sizeof-expression)
        vector_concat(&collisions, collisionsWithCurrentLayer);

        vector_free(collisionsWithCurrentLayer);
    }

    return collisions;
}
