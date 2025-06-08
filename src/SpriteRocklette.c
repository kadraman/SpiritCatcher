#include "Banks/SetAutoBank.h"
#include "main.h"

#include "SpriteManager.h"

#include "ZGBMain.h"
#include "Scroll.h"

#include "Sound.h"

// player sprite pointer declaration, initialized in the SpritePlayer.c
extern Sprite *player_sprite;

typedef struct {
	INT8 vx;
	INT8 vy;
} CUSTOM_DATA;
CHECK_CUSTOM_DATA_SIZE(CUSTOM_DATA); // check that CUSTOM_DATA struct fits the CUSTOM_DATA_SIZE space

void CreateRocklette(UINT16 x, UINT16 y, INT8 vx, INT8 vy) BANKED {
	Sprite* spr = SpriteManagerAdd(SpriteRocklette, x, y);
	CUSTOM_DATA* data = (CUSTOM_DATA*)spr->custom_data;

	data->vx = vx;
	data->vy = vy;

	PlayFx(CHANNEL_1, 10, 0x2C, 0xD8, 0xF3, 0x62, 0xC7);
}

void START() {
}

void UPDATE() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	
	//THIS->x += (INT16)data->vx << delta_time;
	//THIS->y += (INT16)data->vy << delta_time;
	if (TranslateSprite(THIS, (INT16)data->vx << delta_time, (INT16)data->vy << delta_time)) {
		SetFrame(THIS, 1); // set to frame 1 for the rocklette sprite
		// if the sprite is out of bounds, remove it
		SpriteManagerRemove(THIS_IDX);
		return;
	}

	//TODO: make this BANKED for animation
	/*if (CheckCollision(THIS, player_sprite)) {
		SetFrame(THIS, 1); // set to frame 1 for the rocklette sprite
		// if the sprite collides with the player, remove it
		SpriteManagerRemove(THIS_IDX);
		// play a sound effect
		PlayFx(CHANNEL_1, 10, 0x0d, 0xff, 0x7d, 0xc0);
		return;
	}*/
}

void DESTROY() {
}