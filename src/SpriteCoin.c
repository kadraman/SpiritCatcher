#include <stdbool.h>
#include "Banks/SetAutoBank.h"
#include "Sound.h"
#include "main.h"
#include "SpriteManager.h"

#include "SpritePlayer.h"
#include "GameTypes.h"

const UINT8 anim_coin[] = {8, 0, 1, 2, 3, 4, 1, 3, 2};

extern Sprite* player_sprite;
extern UINT8 item_collected;
UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* collectable, ItemType itype) BANKED;
typedef struct {
	UINT16 start_y;
    UINT8 frame;
} CustomData;


void START() {
	CustomData* data = (CustomData*)THIS->custom_data;
	if (IsCollected(THIS) != 255) {
		SpriteManagerRemove(THIS_IDX);
	} else {
		//data->start_y = THIS->y;
		//data->frame = 0;
	}
	SetSpriteAnim(THIS, anim_coin, 30u);
	//SetFrame(THIS, (((THIS->x >> 3) & 0x2) == 0) ? 0 : 1);
}

void UPDATE() {
	CustomData* data = (CustomData*)THIS->custom_data;
	data->frame++;
	//THIS->y = data->start_y + (SIN(data->frame + (UINT8)THIS->x) >> 5);
	if (CheckCollision(THIS, player_sprite)) {
		TakeCollectable(THIS, ITEM_COIN);
		SpriteManagerRemove(THIS_IDX);
		Hud_Update();
	}

}

void DESTROY() {
}