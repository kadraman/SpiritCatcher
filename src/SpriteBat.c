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

const UINT8 anim_bat_move[] = {3, 0, 1, 2};
const UINT8 anim_bat_attack[] = {3, 3, 4, 5};
UINT16 startx, starty;
UINT8 frame_skip = 0;
typedef struct {
	INT16 dx, dy;	// x and y projection of the line
	INT16 err;	    // accumulated error
	INT8  sx, sy;   // x and y coordinate increment
} CUSTOM_DATA;
CHECK_CUSTOM_DATA_SIZE(CUSTOM_DATA); // check that CUSTOM_DATA struct fits the CUSTOM_DATA_SIZE space

void START() {
    CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;
    startx = THIS->x; starty = THIS->y;
	SetSpriteAnim(THIS, anim_bat_move, 5u);
	THIS->lim_x = 160u;
	THIS->lim_y = 160u;
}

void UPDATE() {

	CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;
	data->dx  =  abs(TARGET_X_COORD - THIS->x);
	data->dy  = -abs(TARGET_Y_COORD - THIS->y);
	data->err = data->dx + data->dy; 
	data->sx  = (TARGET_X_COORD > THIS->x) ? 1 : -1;
	data->sy  = (TARGET_Y_COORD > THIS->y) ? 1 : -1;

    // are we close enough to attack?
    if (data->dx < ATTACK_DISTANCE && data->dy < ATTACK_DISTANCE) {
        SetSpriteAnim(THIS, anim_bat_attack, 5u);
        if (frame_skip == ATTACK_DELAY) {
            INT16 e2 = data->err << 1;
            if (e2 > data->dy) { data->err += data->dy; THIS->x += data->sx; }
            if (e2 < data->dx) { data->err += data->dx; THIS->y += data->sy; }
            frame_skip = 0;
        } else {
            frame_skip++;
        }
    } else {
	    SetSpriteAnim(THIS, anim_bat_move, 5u);
        // TODO: return to starting location ?
    }

    if (CheckCollision(THIS, player_sprite)) {
        // TODO: move away after successful attack
        
        //THIS->x = startx;
        //THIS->y = starty;
        //Sprite *spr2 = SpriteManagerAdd(SpriteParticle, THIS->x, THIS->y);
		//SpriteManagerRemove(THIS_IDX);
	}

}

void DESTROY() {
}