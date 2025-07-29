#include <raylib.h>
#include "../../include/render/themes.h"
#include "../../include/game/signal_type.h"

const Color dark_palette[] = {
    [S_BLOCK] = (Color){ 0, 0, 0, 0 },
    [S_RED] = DARK_RED,
    [S_BLUE] = DARK_BLUE,
    [S_ORANGE] = DARK_ORANGE,
    [S_YELLOW] = DARK_YELLOW,
};

const Color light_palette[] = {
    [S_BLOCK] = (Color){ 0, 0, 0, 0 },
    [S_RED] = RED,
    [S_BLUE] = BLUE,
    [S_ORANGE] = ORANGE,
    [S_YELLOW] = YELLOW,
};