#include "Banks/SetAutoBank.h"
#include "main.h"

#include "SpriteManager.h"

#include "ZGBMain.h"
#include "Scroll.h"

#include "Sound.h"

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
	
	THIS->x += (INT16)data->vx << delta_time;
	THIS->y += (INT16)data->vy << delta_time;
}

void DESTROY() {
}