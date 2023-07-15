#pragma once

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define WINDOW_NAME "Game"

#define BACKGROUND_COLOR 0xFF, 0xFF, 0xFF, 0xFF

#define FPS 60

#define FRAME_DURATION (1000 / FPS)

#define GRAVITY 0.9

// NOTE: should not be larger than the smallest solid tile size + shortest
//          character hitbox.
// Otherwise the character will be able to clip trough it.
#define MAX_ACCELERATION 3
