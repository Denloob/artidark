#include "SDL.h"
#include "character.h"
#include "tile.h"
#include "utils.h"
#include "vec.h"
#include <stddef.h>

Character *character_create(SDL_Texture *texture, SDL_FRect hitbox, int scalingFactor)
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
    character->hitbox.x += character->velocity.x;

    character_fixTileClip(character, tiles, x, w);

    character->hitbox.y += character->velocity.y;

    character_fixTileClip(character, tiles, y, h);
}

void character_setVelocity(Character *character, float x, float y)
{
    character->velocity.x = x;
    character->velocity.y = y;
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
