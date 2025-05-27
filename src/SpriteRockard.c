#include "Banks/SetAutoBank.h"
#include "main.h"
#include <stdbool.h>

#include <gb/gb.h>
#include <gbdk/emu_debug.h>

#include "Scroll.h"
#include "SpriteManager.h"

// player sprite pointer declaration, initialized in the SpritePlayer.c
extern Sprite *player_sprite;

// create enemy bullet function
//void CreateEnemyBullet(UINT16 x, UINT16 y, INT8 vx, INT8 vy) BANKED;

// animation frames - set directly below 
const UINT8 anim_rockard_move[] = {4, 0, 1, 2, 1};
#define MAX_FRAMES 4
typedef struct
{
	UINT8 start_x;
	UINT8 firing;
} CUSTOM_DATA;
CHECK_CUSTOM_DATA_SIZE(CUSTOM_DATA); // check that CUSTOM_DATA struct fits the CUSTOM_DATA_SIZE space


void START() {
    CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;
	memset(data, 0, sizeof(CUSTOM_DATA));
	THIS->lim_x = 64u;
	THIS->lim_y = 0u;
	data->start_x = THIS->x;
	SetSpriteAnim(THIS, anim_rockard_move, 2u);

}

void UPDATE() {
    CUSTOM_DATA *data = (CUSTOM_DATA*)THIS->custom_data;
	// mirror the sprite if it is at the end of the animation
	THIS->mirror = (THIS->anim_frame == (UINT8)VECTOR_LEN(THIS->anim_data)-1) ? V_MIRROR : NO_MIRROR;
	// if facing forward or backward, reset firing
	if (THIS->anim_frame == 0 || THIS->anim_frame == 2) {
		data->firing = false;
	}
	// TODO: check if player is within range (Y coordinate)
	// if player is within range, fire a rocklette
	if ((THIS->anim_frame == 1 && !data->firing) && (player_sprite->x < THIS->x)) {
		//EMU_printf("SpriteRockard::UPDATE: firing left\n");
		data->firing = true;
	}
	if ((THIS->anim_frame == (UINT8)VECTOR_LEN(THIS->anim_data)-1 && !data->firing) && (player_sprite->x > THIS->x)) {
		//EMU_printf("SpriteRockard::UPDATE: firing right\n");
		data->firing = true;
	}
}

void DESTROY() {
}

/*
		//SetVisible(THIS, 0);
		//if (THIS->mirror == V_MIRROR && player_sprite->mirror == NO_MIRROR) {
		if (THIS->mirror == V_MIRROR && player_sprite->x > THIS->x) {
			//THIS->x = data->start_x - 5u;
			//CreateEnemyBullet(THIS->x + 12, THIS->y + 6, 1, 0);
			EMU_printf("player_sprite->x: %d\n", player_sprite->x);
		//} else if (THIS->mirror == NO_MIRROR && player_sprite->mirror == V_MIRROR) {
		} else if (THIS->mirror == NO_MIRROR && player_sprite->x < THIS->x) {
			//THIS->x = data->start_x + 5u;
			//CreateEnemyBullet(THIS->x + 4, THIS->y + 6, -1, 0);
			EMU_printf("player_sprite->x: %d\n", player_sprite->x);
		}
*/