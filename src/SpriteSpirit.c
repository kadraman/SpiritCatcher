#include <stdbool.h>
#include <string.h>
#include <gb/gb.h>
#include "Sound.h"
#include "main.h"

#include "Banks/SetAutoBank.h"
#include "Vector.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "Scroll.h"

#include "ZGBMain.h"
#include "GameTypes.h"
#include "SpritePlayer.h"

#include "SpriteSpirit.h"

extern Sprite* lantern_sprite;
UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* collectable, ItemType itype) BANKED;

/*
 * Catch / absorb motion
 * ---------------------
 * Spirit is 16x16; lantern is 8x8. GB hardware cannot scale sprites, so sliding
 * the full body into the lantern will never look like a real absorb on its own.
 * Current code: calm 1px drift of the spirit centre toward the lantern centre,
 * tiny residual bob, no mirror flipping (flips read as jerk on a large body).
 *
 * TODO: catch art — append frames to res/sprites/spirit.png (same sheet), then
 *       wire anim_spirit_caught (and hide the big body at the end):
 *   1. Shocked 16x16 (existing frame 3 is a start)
 *   2. Squash / stretch toward the lantern (~16x16)
 *   3. Half-size wisp (~8x16 or 8x8)
 *   4. Tiny spark / glow (8x8) that sits on the lantern before vanishing
 * Optional: brighter lantern frame when it "eats" the spirit
 *           (res/sprites/lantern.png).
 */
#define PULL_MAX 48u
#define IDLE_AMP 3u
#define CATCH_BOB_AMP 1u /* keep motion soft while caught */

typedef struct {
	UINT16 start_x;
	UINT16 start_y;
	UINT8 pos_counter;
	UINT8 frame_toggle;
	UINT8 caught;
	UINT8 pull;
} CUSTOM_DATA;
ASSERT_CUSTOM_DATA_SIZE(CUSTOM_DATA, 8);

static UINT8 lantern_is_live(void) {
	return (lantern_sprite != NULL && !lantern_sprite->marked_for_removal) ? 1u : 0u;
}

static void spirit_step_toward(UINT16* pos, UINT16 target) {
	if (*pos < target) (*pos)++;
	else if (*pos > target) (*pos)--;
}

static void spirit_set_pos(UINT16 x, UINT16 y) {
	if (scroll_w > 16u && x > scroll_w - 16u) x = scroll_w - 16u;
	if (scroll_h > 16u && y > scroll_h - 16u) y = scroll_h - 16u;
	THIS->x = x;
	THIS->y = y;
}

static void spirit_apply_orbit(CUSTOM_DATA* data, UINT8 amp) {
	INT16 ox = ((INT16)sin_table[data->pos_counter] * (INT16)amp) >> 4;
	INT16 oy = ((INT16)sin_table[(data->pos_counter + 32u) & 127u] * (INT16)amp) >> 4;
	spirit_set_pos(
		(UINT16)((INT16)data->start_x + ox),
		(UINT16)((INT16)data->start_y + oy)
	);
}

void START() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	memset(data, 0, sizeof(CUSTOM_DATA));
	if (IsCollected(THIS) != 255) {
		SpriteManagerRemove(THIS_IDX);
	} else {
		/* Orbit around the sprite's top-left (same convention as before). */
		data->start_x = THIS->x;
		data->start_y = THIS->y;
	}
	SetSpriteAnim(THIS, anim_spirit, 20u);
}

void UPDATE() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	UINT8 amp;
	UINT16 lx, ly;
	UINT16 tx, ty;

	if (!data->caught) {
		data->frame_toggle ^= 1u;
		if (data->frame_toggle) {
			data->pos_counter = (data->pos_counter + 1u) & 127u;
			data->frame_toggle = 0u;
		}
		spirit_apply_orbit(data, IDLE_AMP);

		if (lantern_is_live() && CheckCollision(THIS, lantern_sprite)) {
			data->caught = 1u;
			data->pull = 0u;
			/* Anchor at current drawn position so the catch doesn't jump */
			data->start_x = THIS->x;
			data->start_y = THIS->y;
			SetAnimationLoop(THIS, TRUE);
			SetSpriteAnim(THIS, anim_spirit_caught, 10u);
		}
		return;
	}

	/* Target = lantern centre minus half spirit size → centres line up */
	if (lantern_is_live()) {
		lx = lantern_sprite->x;
		ly = lantern_sprite->y;
		tx = (lx > 4u) ? (UINT16)(lx - 4u) : 0u; /* 8/2 lantern - 16/2 spirit */
		ty = (ly > 4u) ? (UINT16)(ly - 4u) : 0u;
	} else {
		tx = data->start_x;
		ty = data->start_y;
		if (data->pull < (PULL_MAX - 8u)) data->pull = (UINT8)(PULL_MAX - 8u);
	}

	/* One pixel per axis per frame — smooth on a 16x16 body */
	spirit_step_toward(&data->start_x, tx);
	spirit_step_toward(&data->start_y, ty);

	/* Slow the bob; shrink to 0 in the last third */
	data->frame_toggle ^= 1u;
	if (data->frame_toggle) {
		data->pos_counter = (data->pos_counter + 1u) & 127u;
	}
	if (data->pull > (PULL_MAX - (PULL_MAX / 3u))) amp = 0u;
	else amp = CATCH_BOB_AMP;

	spirit_apply_orbit(data, amp);

	data->pull++;
	if (data->pull >= PULL_MAX
		|| (data->start_x == tx && data->start_y == ty && data->pull > 20u)) {
		TakeCollectable(THIS, ITEM_SPIRIT);
		SpriteManagerRemove(THIS_IDX);
	}
}

void DESTROY() {
}
