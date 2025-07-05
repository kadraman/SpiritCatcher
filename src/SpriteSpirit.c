#include <stdbool.h>
#include <gb/gb.h>
#include "Sound.h"
#include "main.h"

#include "Banks/SetAutoBank.h"
#include "Vector.h"
#include "SpriteManager.h"

#include "GameTypes.h"
#include "SpritePlayer.h"

const UINT8 anim_spirit[] = {3, 0, 1, 2};
const INT8 sin_table[128] = {
      0,   4,   6,  10,  12,  16,  18,  22,
     24,  28,  30,  34,  36,  40,  42,  46,
     47,  51,  53,  57,  58,  62,  63,  67,
     68,  72,  73,  77,  77,  81,  81,  85,
     85,  89,  89,  92,  92,  95,  95,  98,
     98, 101, 101, 103, 103, 105, 105, 107,
    107, 108, 108, 109, 109, 110, 110, 111,
    111, 111, 111, 111, 111, 111, 111, 111,
    111, 111, 111, 111, 111, 111, 111, 111,
    110, 110, 109, 109, 108, 108, 107, 107,
    105, 105, 103, 103, 101, 101,  98,  98,
     95,  95,  92,  92,  89,  89,  85,  85,
     81,  81,  77,  77,  73,  72,  68,  67,
     63,  62,  58,  57,  53,  51,  47,  46,
     42,  40,  36,  34,  30,  28,  24,  22,
     18,  16,  12,  10,   6,   4,   0,  -4,
    -6, -10, -12, -16, -18, -22, -24, -28,
   -30, -34, -36, -40, -42, -46, -47, -51,
   -53, -57, -58, -62, -63, -67, -68, -72,
   -73, -77, -77, -81, -81, -85, -85, -89
};

extern Sprite* lantern_sprite;
extern UINT8 item_collected;
UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* collectable, ItemType itype) BANKED;
typedef struct {
	UINT16 start_x;
	UINT16 start_y;
    UINT8 pos_counter;
	UINT8 frame_toggle;
	UINT8 hide_counter;
} CustomData;

void START() {
	CustomData* data = (CustomData*)THIS->custom_data;
	if (IsCollected(THIS) != 255) {
		SpriteManagerRemove(THIS_IDX);
	} else {
		// set start to center of the sprite
		data->start_x = THIS->x - 8;
		data->start_y = THIS->y - 8;
		data->pos_counter = 0;
		data->frame_toggle = 0;
		data->hide_counter = 0;
	}
	SetSpriteAnim(THIS, anim_spirit, 20u);
}

void UPDATE() {
    CustomData* data = (CustomData*)THIS->custom_data;

    // Show/hide logic
    //if (data->hide_counter < 80) { // show for 80 frames
    //    data->hide_counter++;
    //    THIS->visible = 1;
    //} else if (data->hide_counter < 160) { // hide for 80 frames
    //    data->hide_counter++;
    //    THIS->visible = 0;
    //} else { // reset counter
    //    data->hide_counter = 0;
    //}

    // Movement logic (circular, around starting position)
    data->frame_toggle ^= 1;
    if (data->frame_toggle) {
        data->pos_counter = (data->pos_counter + 1) & 127; // 128-step table
        data->frame_toggle = 0;
    }

    // Adjust multiplier for larger/smaller movement radius
    THIS->x = data->start_x + ((sin_table[data->pos_counter] * 3) >> 4);
    THIS->y = data->start_y + ((sin_table[(data->pos_counter + 32) & 127] * 3) >> 4);

    // Only allow collection if visible
    if (CheckCollision(THIS, lantern_sprite) && GetPlayerState() == PLAYER_STATE_CATCH) {
        TakeCollectable(THIS, ITEM_SPIRIT);
        PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
        SpriteManagerRemove(THIS_IDX);
    }
}

void DESTROY() {
}