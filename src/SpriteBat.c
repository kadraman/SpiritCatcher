#include "Banks/SetAutoBank.h"
#include "main.h"
#include <stdlib.h>

#include <gb/gb.h>

#include "Scroll.h"
#include "SpriteManager.h"
#include "ZGBMain.h"

// player sprite pointer declaration, initialized in the SpritePlayer.c
extern Sprite *player_sprite;

// player sprite dimensions
#define PLAYER_WIDTH  16
#define PLAYER_HEIGHT 16
// distance to player until attack
#define ATTACK_DISTANCE 50
#define ATTACK_DELAY    1
// aim to the center of the player sprite
#define TARGET_X_COORD (player_sprite->x + (PLAYER_WIDTH / 2))
#define TARGET_Y_COORD (player_sprite->y + (PLAYER_HEIGHT / 2))
// number of frames to retreat
#define RETREAT_DURATION 20

const UINT8 anim_bat_move[] = {3, 0, 1, 2};
const UINT8 anim_bat_attack[] = {3, 3, 4, 5};
UINT16 startx, starty;
typedef struct {
    INT8 dx, dy;	    // x and y projection of the line
    INT8 err;	        // accumulated error
    INT8  sx, sy;       // x and y coordinate increment
    UINT8 retreating;   // flag to indicate if the bat is retreating
    UINT8 retreat_timer;// timer for retreating
    UINT8 frame_skip;   // moved from file scope to per-bat
} CUSTOM_DATA;
ASSERT_CUSTOM_DATA_SIZE(CUSTOM_DATA, 8);

void START() {
    CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;
    memset(data, 0, sizeof(CUSTOM_DATA));
    SetSpriteAnim(THIS, anim_bat_move, 5u);
    THIS->lim_x = 500u;
    THIS->lim_y = 144u;
    data->retreat_timer = 0; // reset retreating timer
    data->frame_skip = 0; // initialize per-bat frame_skip
}

void UPDATE() {
    CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;

    if (data->retreat_timer > 0) {
        // Move away from player for RETREAT_DURATION frames
        data->retreat_timer++;
        THIS->x -= (data->sx >> 1); // retreat slower: divide by 2
        THIS->y -= (data->sy >> 1); // retreat slower: divide by 2
        SetSpriteAnim(THIS, anim_bat_move, 5u);
        if (data->retreat_timer >= RETREAT_DURATION) {
            data->retreat_timer = 0;
        }
        return;
    }

    data->dx  =  abs(TARGET_X_COORD - THIS->x);
    data->dy  = -abs(TARGET_Y_COORD - THIS->y);
    data->err = data->dx + data->dy; 
    data->sx  = (TARGET_X_COORD > THIS->x) ? 1 : -1;
    data->sy  = (TARGET_Y_COORD > THIS->y) ? 1 : -1;

    // are we close enough to attack?
    if (data->dx < ATTACK_DISTANCE && data->dy < ATTACK_DISTANCE) {
        SetSpriteAnim(THIS, anim_bat_attack, 5u);
        if (data->frame_skip == ATTACK_DELAY) {
            INT16 e2 = data->err << 1;
            if (e2 > data->dy) { data->err += data->dy; THIS->x += data->sx << delta_time; }
            if (e2 < data->dx) { data->err += data->dx; THIS->y += data->sy << delta_time; }
            data->frame_skip = 0;
        } else {
            data->frame_skip++;
        }
    } else {
        SetSpriteAnim(THIS, anim_bat_move, 5u);
    }

    if (CheckCollision(THIS, player_sprite)) {
        //EMU_printf("SpriteBat::UPDATE: player hit!\n");
        SetSpriteAnim(THIS, anim_bat_move, 5u);
        // start retreating
        data->retreat_timer = 1; 
    }

}

void DESTROY() {
}