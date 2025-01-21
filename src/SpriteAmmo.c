#include "Banks/SetAutoBank.h"
#include "Sound.h"
#include "main.h"
#include "SpriteManager.h"

#include "SpritePlayer.h"
#include "GameTypes.h"

const UINT8 anim_ammo[] = {4, 0, 1, 2, 3};

extern Sprite* player_sprite;
extern UINT8 item_collected;
UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* collectable, ItemType itype) BANKED;
typedef struct {
	UINT16 start_y;
} CustomData; // TBD

void START() {
	CustomData* data = (CustomData*)THIS->custom_data;
	if (IsCollected(THIS) != 255) {
		SpriteManagerRemove(THIS_IDX);
	}
	SetSpriteAnim(THIS, anim_ammo, 20u);
}

void UPDATE() {
	CustomData* data = (CustomData*)THIS->custom_data;
	if (CheckCollision(THIS, player_sprite)) {
		TakeCollectable(THIS, ITEM_BULLET);
		PlayerData* player_data = (PlayerData*)player_sprite->custom_data;
		//player_data->bullets = player_data->bullets + 6;
		PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
		SpriteManagerRemove(THIS_IDX);
		Hud_Update();
	}
}

void DESTROY() {
}