#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>
#include <gbdk/emu_debug.h>

#include "Scroll.h"
#include "SpriteManager.h"

// player sprite pointer declaration, initialized in the SpritePlayer.c
extern Sprite *player_sprite;

// create enemy bullet function
void CreateEnemyBullet(UINT16 x, UINT16 y, INT8 vx, INT8 vy) BANKED;

// animation frames - set directly below 
const UINT8 anim_rockard_move[] = {4, 0, 1, 2, 1};
struct EnemyInfo
{
    UINT8 wait;
	UINT8 start_x;
};

void START() {
	// TBD: get player position to determine facing directory
	struct EnemyInfo* data = (struct EnemyInfo*)THIS->custom_data;
	data->wait = 0;
	//SetSpriteAnim(THIS, anim_rockard_move, 5u);
	//THIS->lim_x = 160u;
	//THIS->lim_y = 160u;
	data->start_x = THIS->x;
}

void UPDATE() {
	struct EnemyInfo* data = (struct EnemyInfo*)THIS->custom_data;
	data->wait += 1 << delta_time;
	if (data->wait > 160u) {
		SetVisible(THIS, 0);
		//if (THIS->mirror == V_MIRROR && player_sprite->mirror == NO_MIRROR) {
		if (THIS->mirror == V_MIRROR && player_sprite->x > THIS->x) {
			//THIS->x = data->start_x - 5u;
			CreateEnemyBullet(THIS->x + 12, THIS->y + 6, 1, 0);
			EMU_printf("player_sprite->x: %d\n", player_sprite->x);
		//} else if (THIS->mirror == NO_MIRROR && player_sprite->mirror == V_MIRROR) {
		} else if (THIS->mirror == NO_MIRROR && player_sprite->x < THIS->x) {
			//THIS->x = data->start_x + 5u;
			CreateEnemyBullet(THIS->x + 4, THIS->y + 6, -1, 0);
			EMU_printf("player_sprite->x: %d\n", player_sprite->x);
		}
		data->wait = 0;
	}
	SetVisible(THIS, 1);
	THIS->x = data->start_x;

	if (data->wait < 40u) {
		SetFrame(THIS, 0);
	} else if (data->wait > 40u && data->wait < 80u) {
		THIS->mirror = V_MIRROR;
		SetFrame(THIS, 1);
	} else if (data->wait > 80u && data->wait < 120u) {
		SetFrame(THIS, 2);
	} else if (data->wait > 120u && data->wait < 160u) {
		THIS->mirror = NO_MIRROR;
		SetFrame(THIS, 1);
	} else {
		// ignore as reset above
	}
}

void DESTROY() {
}