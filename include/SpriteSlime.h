#ifndef SPRITESLIME_H
#define SPRITESLIME_H

#include <gb/gb.h>
#include "main.h"
#include "SpriteManager.h"

// player sprite pointer declaration, initialized in the SpritePlayer.c
extern Sprite *player_sprite;

const UINT8 anim_slime_wait[] = {4, 0, 1, 2, 3};
const UINT8 anim_slime_move[] = {2, 2, 2};

// Precomputed tables for fastest runtime on Game Boy
#define SLIME_DURATION 120u
#define MOVE_WIDTH 48
#define MOVE_HEIGHT 16

// slime_dx_table: dx = floor(MOVE_WIDTH * t / SLIME_DURATION) for t=0..SLIME_DURATION
static const INT16 slime_dx_table[SLIME_DURATION + 1] = {
    0, 0, 0, 1, 1, 2, 2, 2, 
    3, 3, 4, 4, 4, 5, 5, 6, 
    6, 6, 7, 7, 8, 8, 8, 9, 
    9, 10, 10, 10, 11, 11, 12, 12, 
    12, 13, 13, 14, 14, 14, 15, 15, 
    16, 16, 16, 17, 17, 18, 18, 18, 
    19, 19, 20, 20, 20, 21, 21, 22, 
    22, 22, 23, 23, 24, 24, 24, 25, 
    25, 26, 26, 26, 27, 27, 28, 28, 
    28, 29, 29, 30, 30, 30, 31, 31, 
    32, 32, 32, 33, 33, 34, 34, 34, 
    35, 35, 36, 36, 36, 37, 37, 38, 
    38, 38, 39, 39, 40, 40, 40, 41, 
    41, 42, 42, 42, 43, 43, 44, 44, 
    44, 45, 45, 46, 46, 46, 47, 47, 
    48 
};

// slime_dy_table: dy = floor((4*MOVE_HEIGHT*t*(D-t)) / (D*D)) for t=0..SLIME_DURATION
static const INT16 slime_dy_table[SLIME_DURATION + 1] = {
    0, 0, 1, 1, 2, 2, 3, 3, 
    3, 4, 4, 5, 5, 6, 6, 7, 
    7, 7, 8, 8, 8, 9, 9, 9, 
    10, 10, 10, 11, 11, 11, 12, 12, 
    12, 12, 12, 13, 13, 13, 13, 14, 
    14, 14, 14, 14, 14, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 14, 14, 14, 14, 
    14, 14, 13, 13, 13, 13, 12, 12, 
    12, 12, 12, 11, 11, 11, 10, 10, 
    10, 9, 9, 9, 8, 8, 8, 7, 
    7, 7, 6, 6, 5, 5, 4, 4, 
    3, 3, 3, 2, 2, 1, 1, 0, 
    0 
};

#endif // SPRITESLIME_H
