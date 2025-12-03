#include <stdbool.h>
#include <gb/gb.h>
#include "Sound.h"
#include "main.h"

#include "Banks/SetAutoBank.h"
#include "Vector.h"
#include "SpriteManager.h"

#include "ZGBMain.h"
#include "GameTypes.h"
#include "SpritePlayer.h"

#include "SpriteSpirit.h"

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
	UINT8 caught; 
} CUSTOM_DATA;
ASSERT_CUSTOM_DATA_SIZE(CUSTOM_DATA, 8);

void START() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
    memset(data, 0, sizeof(CUSTOM_DATA));
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
    CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;

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

    /* Movement algorithm:
     * - Uses a 128-step lookup table `sin_table[]` as a compact waveform.
     * - `pos_counter` indexes the table (0..127). `frame_toggle` advances
     *   `pos_counter` only once every two UPDATE() calls so the spirit moves
     *   at half the update rate (smoother, slower bobbing).
     * - Horizontal offset is `sin_table[pos]` scaled by 3/16 (multiply by 3
     *   then shift right 4). Vertical offset uses a 90-degree phase shift
     *   via `(pos + 32) & 127`, so X/Y combine into a circular/elliptical
     *   path around the stored `start_x,start_y` center.
     * - The table stores signed INT8 samples so runtime cost is just a
     *   table lookup + small integer math — suitable for constrained
     *   hardware (no floating point or expensive trig at runtime).
     */
    data->frame_toggle ^= 1;
    if (data->frame_toggle) {
        data->pos_counter = (data->pos_counter + 1) & 127; // 128-step table
        data->frame_toggle = 0;
    }

    /* apply scaled table samples to position (start_x/start_y are sprite
     * center offsets set in START()) */
    THIS->x = data->start_x + ((sin_table[data->pos_counter] * 3) >> 4);
    THIS->y = data->start_y + ((sin_table[(data->pos_counter + 32) & 127] * 3) >> 4);

    if (!data->caught) {
        if (CheckCollision(THIS, lantern_sprite) && GetPlayerState() == PLAYER_STATE_CATCH) {
            SetSpriteAnim(THIS, anim_spirit_caught, 20u);
            data->caught = 1;
        }
    } else {
        // Wait for animation to finish
        if (THIS->anim_frame == VECTOR_LEN(anim_spirit_caught) - 1) {
            TakeCollectable(THIS, ITEM_SPIRIT);
            PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
            SpriteManagerRemove(THIS_IDX);
        }
    }
}

void DESTROY() {
}