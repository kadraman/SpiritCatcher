#include <stdbool.h>
#include "Sound.h"
#include "main.h"

#include "Banks/SetAutoBank.h"
#include "Vector.h"
#include "SpriteManager.h"

// TODO: refactor ItemType from below
#include "SpritePlayer.h"

const UINT8 anim_spirit[] = VECTOR(3, 0, 1, 2);
struct SpiritInfo {
	UINT16 start_y;
    UINT8 frame;
};

UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* collectable, ItemType itype) BANKED;

extern Sprite* player_sprite;
extern UINT8 item_collected;

void START() {
	struct SpiritInfo* data = (struct SpiritInfo*)THIS->custom_data;
	if (IsCollected(THIS) != 255) {
		SpriteManagerRemove(THIS_IDX);
	} else {
		//data->start_y = THIS->y;
		//data->frame = 0;
	}
	SetSpriteAnim(THIS, anim_spirit, DEFAULT_ANIM_SPEED);
	//SetFrame(THIS, (((THIS->x >> 3) & 0x2) == 0) ? 0 : 1);
}

void UPDATE() {
	struct SpiritInfo* data = (struct SpiritInfo*)THIS->custom_data;
	//THIS->y = data->start_y + (SIN(data->frame + (UINT8)THIS->x) >> 5);
	if (CheckCollision(THIS, player_sprite)) {
		TakeCollectable(THIS, ITEM_SPIRIT);
		SpriteManagerRemove(THIS_IDX);
		Hud_Update();
	}
	//data->frame++;
}

void DESTROY() {
}