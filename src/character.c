#include "SDL.h"
#include "character.h"

void character_draw(const Character *character, SDL_Renderer *renderer)
{
    SDL_RenderDrawRectF(renderer, &character->hitbox);
}

void character_tick(Character *character)
{
    character_applyVelocity(character);   
}

void character_applyVelocity(Character *character)
{
    character->hitbox.x += character->velocity.x;
    character->hitbox.y += character->velocity.y;
}

void character_applyGravity(Character *character, float gravity)
{
    character->velocity.y += gravity;
}
