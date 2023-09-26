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
 * @param movement_delta Delta of player's movement.
 * @param pos_axis `x` or `y`
 * @param size_axis `w` or `h`
 */
#define character_apply_collisions_after_movement(                             \
    character, collisions, movement_delta, pos_axis, size_axis)                \
    {                                                                          \
        if (vector_size(collisions))                                           \
            (character->velocity).pos_axis = 0;                                \
        character_unset_collision(                                             \
            character, (CHARACTER_COLLISION_##pos_axis##_NEGATIVE) |           \
                           (CHARACTER_COLLISION_##pos_axis##_POSITIVE));       \
        CharacterCollisionDirection collision_direction = 0;                   \
                                                                               \
        for (size_t i = 0; i < vector_size(collisions); i++)                   \
        {                                                                      \
            if (movement_delta < 0)                                            \
            {                                                                  \
                (character->hitbox).pos_axis =                                 \
                    (collisions[i]->hitbox).pos_axis +                         \
                    (collisions[i]->hitbox).size_axis;                         \
                                                                               \
                collision_direction |=                                         \
                    (CHARACTER_COLLISION_##pos_axis##_NEGATIVE);               \
            }                                                                  \
            else if (movement_delta > 0)                                       \
            {                                                                  \
                (character->hitbox).pos_axis =                                 \
                    (collisions[i]->hitbox).pos_axis -                         \
                    (character->hitbox).size_axis;                             \
                                                                               \
                collision_direction |=                                         \
                    (CHARACTER_COLLISION_##pos_axis##_POSITIVE);               \
            }                                                                  \
        }                                                                      \
                                                                               \
        character_set_collision(character, collision_direction);               \
    }

/** Clamps the velocity of character on given axis between -max_velocity and
 *      +max_velocity.
 */
#define character_clamp_velocity_on_axis(character, axis, max_velocity)        \
    {                                                                          \
        (character->velocity).axis = SDL_clamp((character->velocity).axis,     \
                                               -max_velocity, max_velocity);   \
    }

#define character_tick_movement_on_axis(                                       \
    character, layers, apply_movement_func, pos_axis, size_axis)               \
    {                                                                          \
        float pos_before_movement = (character)->hitbox.pos_axis;              \
        apply_movement_func(character);                                        \
        float movement_delta =                                                 \
            (character)->hitbox.pos_axis - pos_before_movement;                \
                                                                               \
        VecTile *collisions =                                                  \
            character_find_collisions_with_layer_tiles(character, layers);     \
        character_apply_collisions_after_movement(                             \
            character, collisions, movement_delta, pos_axis, size_axis);       \
        vector_free(collisions);                                               \
    }

Character *character_create(SDL_Texture *texture, SDL_FRect hitbox, int speed,
                            int jump_strength, int scaling_factor)
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

    hitbox.w *= scaling_factor;
    hitbox.h *= scaling_factor;

    character->hitbox = hitbox;
    character->velocity = (SDL_FPoint){0, 0};
    character->movement_direction = 0;
    character->speed = speed;
    character->jump_strength = jump_strength;

    return character;
}

void character_destroy(Character *character)
{
    free(character);
}

void character_clamp_velocity(Character *character, float max_velocity)
{
    character_clamp_velocity_on_axis(character, x, max_velocity);
    character_clamp_velocity_on_axis(character, y, max_velocity);
}

void character_draw(const Character *character, SDL_Renderer *renderer,
                    SDL_FPoint *offset)
{
    renderer_render_copy_with_offset_f(renderer, character->texture, NULL,
                                       &character->hitbox, offset);
}

SDL_FPoint character_get_position(const Character *character)
{
    return (SDL_FPoint){character->hitbox.x, character->hitbox.y};
}

void character_tick(Character *character, const VecLevelLayer layers,
                    float max_acceleration)
{
    character_clamp_velocity(character, max_acceleration);

    character_tick_movement(character, layers);
}

/*
 * @brief Applies the horizontal movement the character, without checking for
 *          any collisions.
 * @see character_tick_horizontal_movement
 */
void character_apply_horizontal_movement(Character *character)
{
    character->hitbox.x += character->velocity.x;
    if (character->movement_direction & CHARACTER_MOVE_RIGHT)
        character->hitbox.x += character->speed;
    if (character->movement_direction & CHARACTER_MOVE_LEFT)
        character->hitbox.x -= character->speed;
}

/* @see character_tick_movement */
void character_tick_horizontal_movement(Character *character,
                                        const VecLevelLayer layers)
{
    character_tick_movement_on_axis(character, layers,
                                    character_apply_horizontal_movement, x, w);
}

/*
 * @brief Applies the vertical movement the character, without checking for
 *          any collisions.
 * @see character_tick_vertical_movement
 */
void character_apply_vertical_movement(Character *character)
{
    character->hitbox.y += character->velocity.y;
}

/* @see character_tick_movement */
void character_tick_vertical_movement(Character *character,
                                      const VecLevelLayer layers)
{
    character_tick_movement_on_axis(character, layers,
                                    character_apply_vertical_movement, y, h);
}

void character_tick_movement(Character *character, const VecLevelLayer layers)
{
    character_tick_vertical_movement(character, layers);
    character_tick_horizontal_movement(character, layers);
}

void character_set_movement(Character *character,
                            CharacterMovementDirection direction)
{
    character->movement_direction |= direction;
}

void character_unset_movement(Character *character,
                              CharacterMovementDirection direction)
{
    character->movement_direction &= ~direction;
}

void character_set_collision(Character *character,
                             CharacterCollisionDirection direction)
{
    character->collisions |= direction;
}

void character_unset_collision(Character *character,
                               CharacterCollisionDirection direction)
{
    character->collisions &= ~direction;
}

bool character_is_on_ground(Character *character)
{
    return character->collisions & CHARACTER_COLLISION_BOTTOM;
}

/**
 * @brief Makes the character jump if they are on ground.
 */
void character_jump(Character *character)
{
    if (character_is_on_ground(character))
        character->velocity.y -= character->jump_strength;
}

void character_handle_keyboard_event(Character *character,
                                   SDL_KeyboardEvent *event)
{
    SDL_Keycode keycode = event->keysym.sym;
    CharacterMovementDirection movement_direction = 0;
    switch (keycode)
    {
        case SDLK_SPACE:
            character_jump(character);
            return;
        case SDLK_d:
        case SDLK_RIGHT:
            movement_direction = CHARACTER_MOVE_RIGHT;
            break;
        case SDLK_a:
        case SDLK_LEFT:
            movement_direction = CHARACTER_MOVE_LEFT;
            break;
        default:
            return;
    }

    MovementFunction movement_function = event->type == SDL_KEYDOWN
                                             ? character_set_movement
                                             : character_unset_movement;

    movement_function(character, movement_direction);
}

void character_apply_gravity(Character *character, float gravity)
{
    character->velocity.y += gravity;
}

VecTile *character_find_collisions(const Character *character,
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

VecTile *character_find_collisions_with_layer_tiles(const Character *character,
                                                    const VecLevelLayer layers)
{
    VecTile *collisions = vector_create();
    for (size_t i = 0; i < vector_size(layers); i++)
    {
        VecTile *collisions_with_current_layer =
            character_find_collisions(character, layers[i]->tiles);

        // NOLINTNEXTLINE(bugprone-sizeof-expression)
        vector_concat(&collisions, collisions_with_current_layer);

        vector_free(collisions_with_current_layer);
    }

    return collisions;
}
