#include <stdbool.h>
#include "Banks/SetAutoBank.h"
#include "Sound.h"
#include "main.h"
#include "SpriteManager.h"

#include "GameTypes.h"

const UINT8 anim_mana[] = {1, 0};

extern Sprite* player_sprite;
extern UINT8 item_collected;
UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* collectable, ItemType itype) BANKED;
typedef struct {
	UINT16 start_y;
    UINT8 pos_counter;
} CustomData;

void START() {
	CustomData* data = (CustomData*)THIS->custom_data;
	if (IsCollected(THIS) != 255) {
		SpriteManagerRemove(THIS_IDX);
	} else {
		data->start_y = THIS->y;
		data->pos_counter = 0;
	}
	SetSpriteAnim(THIS, anim_mana, 20u);
}

void UPDATE() {
	CustomData* data = (CustomData*)THIS->custom_data;
	// bounce item's Y position
	data->pos_counter++;
	if (data->pos_counter == 1) {
		THIS->y = THIS->y - 1;
	} else if (data->pos_counter == 30) {
		THIS->y = THIS->y + 1;
	} else if (data->pos_counter > 60) {
		data->pos_counter = 0;
	}
	if (CheckCollision(THIS, player_sprite)) {
		TakeCollectable(THIS, ITEM_MANA);
		PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
		SpriteManagerRemove(THIS_IDX);
	}
}

void DESTROY() {
}