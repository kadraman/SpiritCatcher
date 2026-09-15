#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>
#include <string.h>

#include "Scroll.h"
#include "SpriteManager.h"
#include "StateGame.h"

#include "ZGBMain.h"
#include "SpriteSlime.h"

#define WAIT_HOP    0u
#define WAIT_IDLE   1u
#define WAIT_FALL   2u
#define WAIT_ESCAPE 3u

#define FALL_PERIOD 2u

typedef struct {
	INT16 last_cum_dx;
	INT16 last_cum_dy;
	UINT8 ticks;
	INT8 dir;
	UINT8 wait;
} CUSTOM_DATA;
CHECK_CUSTOM_DATA_SIZE(CUSTOM_DATA);

static UINT8 TranslateSpriteI16(Sprite* spr, INT16 dx, INT16 dy) {
	UINT8 col = 0;
	while (dx != 0 || dy != 0) {
		INT8 sx = (dx > 127) ? 127 : (dx < -128) ? (INT8)-128 : (INT8)dx;
		INT8 sy = (dy > 127) ? 127 : (dy < -128) ? (INT8)-128 : (INT8)dy;
		col = TranslateSprite(spr, sx, sy);
		dx -= sx;
		dy -= sy;
		if (col) break;
	}
	return col;
}

static UINT8 slime_is_water(UINT8 tile) {
	return (tile == TILE_INDEX_WATER_1
		|| tile == TILE_INDEX_WATER_2
		|| tile == TILE_INDEX_WATER_3);
}

/* Mushroom-style foot row for a 16px sprite. */
static UINT8 slime_tile_at(UINT16 px, UINT8 row_add) {
	return GetScrollTile(px >> 3, (THIS->y >> 3) + row_add);
}

/* Solid ground you can stand on — not empty, not water. */
static UINT8 slime_safe_floor_at(UINT16 px) {
	UINT8 t = slime_tile_at(px, 2u);
	/* Water often sits one row lower than the shore — check both */
	if (slime_is_water(t) || slime_is_water(slime_tile_at(px, 3u))) return 0u;
	return scroll_collisions[t] ? 1u : 0u;
}

static UINT8 slime_on_floor(void) {
	return slime_safe_floor_at(THIS->x + (THIS->coll_w >> 1));
}

static UINT8 slime_on_water(void) {
	UINT16 cx = THIS->x + (THIS->coll_w >> 1);
	return (slime_is_water(slime_tile_at(cx, 2u))
		|| slime_is_water(slime_tile_at(cx, 3u))) ? 1u : 0u;
}

static UINT8 slime_water_under(UINT16 px) {
	return (slime_is_water(slime_tile_at(px, 2u))
		|| slime_is_water(slime_tile_at(px, 3u))) ? 1u : 0u;
}

/*
 * Turn away from water / walls before hopping.
 * Need safe floor just ahead; reject if hop mid/landing crosses water.
 */
static UINT8 slime_dir_blocked(INT8 dir) {
	UINT16 near_x, mid_x, end_x;
	UINT8 body;

	if (dir > 0) {
		near_x = THIS->x + THIS->coll_w + 4u;
		mid_x = THIS->x + THIS->coll_w + 24u;
		end_x = THIS->x + THIS->coll_w + (UINT8)MOVE_WIDTH;
	} else {
		if (THIS->x < (UINT8)MOVE_WIDTH) return 1u;
		near_x = THIS->x - 4u;
		mid_x = THIS->x - 24u;
		end_x = THIS->x - (UINT8)MOVE_WIDTH;
	}

	if (!slime_safe_floor_at(near_x)) return 1u;
	if (slime_water_under(mid_x) || slime_water_under(end_x)) return 1u;

	body = slime_tile_at(near_x, 1u);
	if (scroll_collisions[body] && !slime_is_water(body)) return 1u;

	return 0u;
}

/* Pick a horizontal dir that still has safe floor nearby (for escape). */
static INT8 slime_escape_dir(INT8 preferred) {
	UINT8 d;
	for (d = 8u; d <= 40u; d = (UINT8)(d + 8u)) {
		if (preferred > 0) {
			if (slime_safe_floor_at(THIS->x + THIS->coll_w + d)) return 1;
			if (THIS->x >= d && slime_safe_floor_at(THIS->x - d)) return -1;
		} else {
			if (THIS->x >= d && slime_safe_floor_at(THIS->x - d)) return -1;
			if (slime_safe_floor_at(THIS->x + THIS->coll_w + d)) return 1;
		}
	}
	return preferred;
}

static void slime_set_dir(CUSTOM_DATA* data, INT8 dir) {
	data->dir = dir;
	THIS->mirror = (dir < 0) ? V_MIRROR : NO_MIRROR;
}

static void slime_enter_idle(CUSTOM_DATA* data) {
	data->last_cum_dx = 0;
	data->last_cum_dy = 0;
	data->ticks = 0u;
	data->wait = WAIT_IDLE;
	SetSpriteAnim(THIS, anim_slime_wait, 5u);
}

static void slime_enter_hop(CUSTOM_DATA* data) {
	data->ticks = 0u;
	data->last_cum_dx = 0;
	data->last_cum_dy = 0;
	data->wait = WAIT_HOP;
	SetSpriteAnim(THIS, anim_slime_move, 5u);
}

static void slime_enter_fall(CUSTOM_DATA* data) {
	data->ticks = 0u;
	data->wait = WAIT_FALL;
	SetSpriteAnim(THIS, anim_slime_move, 5u);
}

static void slime_enter_escape(CUSTOM_DATA* data, INT8 dir) {
	slime_set_dir(data, slime_escape_dir(dir));
	data->ticks = 0u;
	data->wait = WAIT_ESCAPE;
	SetSpriteAnim(THIS, anim_slime_move, 5u);
}

void START() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	if (IsMapEntityCleared(THIS) != 255) {
		SpriteManagerRemove(THIS_IDX);
		return;
	}
	memset(data, 0, sizeof(CUSTOM_DATA));
	data->wait = WAIT_IDLE;
	slime_set_dir(data, 1);
	SetSpriteAnim(THIS, anim_slime_wait, 5u);
	THIS->lim_x = 500u;
	THIS->lim_y = 144u;
	if (player_sprite != NULL && player_sprite->x < THIS->x) {
		slime_set_dir(data, -1);
	}
}

void UPDATE() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	UINT8 col;

	if (data->wait == WAIT_ESCAPE) {
		TranslateSprite(THIS, data->dir, 0);
		data->ticks++;
		if (slime_on_floor()) {
			slime_enter_idle(data);
			return;
		}
		/* Re-aim toward floor occasionally — don't ping-pong every frame */
		if (data->ticks == 12u) {
			slime_set_dir(data, slime_escape_dir(data->dir));
		}
		if (data->ticks >= 48u) {
			slime_enter_fall(data);
		}
		return;
	}

	if (data->wait == WAIT_FALL) {
		data->ticks++;
		if ((data->ticks % FALL_PERIOD) == 0u) {
			col = TranslateSprite(THIS, 0, 1);
			if (col) {
				if (slime_is_water(col) || slime_on_water()) {
					slime_enter_escape(data, (INT8)(-data->dir));
				} else {
					slime_enter_idle(data);
				}
				return;
			}
		}
		if (slime_on_water()) {
			slime_enter_escape(data, (INT8)(-data->dir));
			return;
		}
		if (THIS->y > scroll_h) SpriteManagerRemove(THIS_IDX);
		return;
	}

	if (slime_on_water()) {
		slime_enter_escape(data, (INT8)(-data->dir));
		return;
	}

	if (data->wait == WAIT_IDLE) {
		if (!slime_on_floor()) {
			slime_enter_fall(data);
			return;
		}

		data->ticks++;
		if (data->ticks < (UINT8)SLIME_DURATION) return;

		/* Edge of water / gap / wall → face inland and wait (do NOT hop) */
		if (slime_dir_blocked(data->dir)) {
			slime_set_dir(data, (INT8)(-data->dir));
			data->ticks = 0u;
			return;
		}
		slime_enter_hop(data);
		return;
	}

	/* Hop — abort immediately if we lose safe floor under the lead foot */
	{
		UINT16 t = data->ticks;
		INT16 dx = slime_dx_table[t];
		INT16 dy = slime_dy_table[t];
		INT16 cur_dx = (INT16)(data->dir * dx);
		INT16 cur_dy = (INT16)(-dy);
		UINT16 lead_x;

		lead_x = (data->dir > 0)
			? (THIS->x + THIS->coll_w + 4u)
			: (THIS->x - 4u);

		/* Still on the ground portion of the hop — refuse to continue into hazard */
		if (dy <= 2 && !slime_safe_floor_at(lead_x)) {
			slime_set_dir(data, (INT8)(-data->dir));
			slime_enter_idle(data);
			return;
		}

		col = TranslateSpriteI16(THIS,
			cur_dx - data->last_cum_dx,
			cur_dy - data->last_cum_dy);

		if (col) {
			if (slime_is_water(col)) {
				slime_enter_escape(data, (INT8)(-data->dir));
			} else {
				slime_set_dir(data, (INT8)(-data->dir));
				slime_enter_idle(data);
			}
			return;
		}

		if (player_sprite != NULL && CheckCollision(THIS, player_sprite)) {
			slime_enter_idle(data);
			return;
		}

		data->last_cum_dx = cur_dx;
		data->last_cum_dy = cur_dy;
		data->ticks++;

		if (slime_on_water()) {
			slime_enter_escape(data, (INT8)(-data->dir));
			return;
		}

		if (data->ticks > (UINT8)(SLIME_DURATION >> 1) && !slime_on_floor()) {
			slime_enter_fall(data);
			return;
		}

		if (data->ticks >= (UINT8)SLIME_DURATION) {
			if (slime_on_floor()) slime_enter_idle(data);
			else slime_enter_fall(data);
		}
	}
}

void DESTROY() {
}
