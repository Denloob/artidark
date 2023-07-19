#include "SDL.h"
#include "character.h"
#include "tile.h"
#include "utils.h"
#include "vec.h"
#include <stddef.h>

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

Character *character_create(SDL_Texture *texture, SDL_FRect hitbox, int speed,
                            int scalingFactor)
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

void character_draw(const Character *character, SDL_Renderer *renderer)
{
    SDL_RenderCopyF(renderer, character->texture, NULL, &character->hitbox);
}

void character_tick(Character *character, const VecTile tiles,
                    float max_acceleration)
{
    character_clamp_velocity(character, max_acceleration);

    character_tickMovement(character, tiles);
}

void character_tickMovement(Character *character, const VecTile tiles)
{
    float posBeforeMovement = character->hitbox.x;
    character->hitbox.x += character->velocity.x;

    if (character->movementDirection & CHARACTER_MOVE_RIGHT)
        character->hitbox.x += character->speed;
    if (character->movementDirection & CHARACTER_MOVE_LEFT)
        character->hitbox.x -= character->speed;

    float movementDelta = character->hitbox.x - posBeforeMovement;
    character_fixTileClip(character, tiles, movementDelta, x, w);

    posBeforeMovement = character->hitbox.y;
    character->hitbox.y += character->velocity.y;

    movementDelta = character->hitbox.y - posBeforeMovement;
    character_fixTileClip(character, tiles, movementDelta, y, h);
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

void character_handleKeyboardEvent(Character *character,
                                   SDL_KeyboardEvent *event)
{
    SDL_Keycode keycode = event->keysym.sym;
    CharacterMovementDirection movementDirection = 0;
    switch (keycode)
    {
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
