#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>
#include <string.h>

#include "Scroll.h"
#include "SpriteManager.h"
#include "StateGame.h"

#include "ZGBMain.h"
#include "SpriteSlime.h"

#define WAIT_HOP   0u
#define WAIT_IDLE  1u
#define WAIT_REV   2u
#define WAIT_FALL  3u

#define FALL_VY_MAX 4u
/* How far below the feet we still treat as "supported" during a hop */
#define GROUND_LOOKAHEAD (MOVE_HEIGHT + 8)

typedef struct {
	INT16 last_cum_dx;
	INT16 last_cum_dy;
	UINT8 ticks;
	INT8 dir;  /* +1 right, -1 left */
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
		|| tile == TILE_INDEX_WATER_3) ? 1u : 0u;
}

static UINT8 slime_is_solid(UINT8 tile) {
	return scroll_collisions[tile] ? 1u : 0u;
}

static UINT8 slime_is_floor(UINT8 tile) {
	return (slime_is_solid(tile) && !slime_is_water(tile)) ? 1u : 0u;
}

static UINT8 slime_tile_xy(UINT16 px, UINT16 py) {
	return GetScrollTile(px >> 3, py >> 3);
}

/* Probe one step ahead: 0 = ok, 1 = turn (wall / water), 2 = ledge (fall). */
static UINT8 slime_probe_ahead(INT8 dir) {
	UINT16 ax = (dir > 0) ? (THIS->x + THIS->coll_w + 2u) : (THIS->x - 2u);
	UINT16 mid_y = THIS->y + (THIS->coll_h >> 1);
	UINT16 below_y = THIS->y + THIS->coll_h + 1u;
	UINT8 wall = slime_tile_xy(ax, mid_y);
	UINT8 below = slime_tile_xy(ax, below_y);

	if (slime_is_water(below) || slime_is_water(wall)) return 1u;
	if (slime_is_floor(wall)) return 1u; /* raised ground / wall */
	if (!slime_is_solid(below)) return 2u; /* ledge — fall, don't turn */
	return 0u;
}

/*
 * Under-feet support:
 *   0 = standing on / above real floor
 *   1 = water is the surface under us (hazard)
 *   2 = void — fall
 *
 * Important: if a floor tile is at the feet, that wins even when water exists
 * further down (avoids flip-loop on shore platforms).
 */
static UINT8 slime_support_below(void) {
	UINT16 fx = THIS->x + (THIS->coll_w >> 1);
	UINT16 fy = THIS->y + THIS->coll_h;
	UINT8 t_feet = slime_tile_xy(fx, fy);
	UINT8 t_up = slime_tile_xy(fx, (fy > 0u) ? (UINT16)(fy - 1u) : 0u);
	UINT8 i;

	if (slime_is_floor(t_feet) || slime_is_floor(t_up)) return 0u;

	/* First non-empty surface below */
	for (i = 0u; i <= (UINT8)GROUND_LOOKAHEAD; i = (UINT8)(i + 8u)) {
		UINT8 t = slime_tile_xy(fx, (UINT16)(fy + i));
		if (slime_is_water(t)) return 1u;
		if (slime_is_floor(t)) return 0u;
	}
	return 2u;
}

static void slime_flip(CUSTOM_DATA* data) {
	data->dir = (INT8)(-data->dir);
	THIS->mirror = (data->dir == -1) ? V_MIRROR : NO_MIRROR;
}

static void slime_nudge_forward(CUSTOM_DATA* data) {
	/* Step away from a hazard so the next probe isn't still in water */
	if (data->dir > 0) TranslateSprite(THIS, 2, 0);
	else TranslateSprite(THIS, -2, 0);
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
	data->dir = (THIS->mirror == V_MIRROR) ? (INT8)-1 : (INT8)1;
	SetSpriteAnim(THIS, anim_slime_move, 5u);
}

static void slime_enter_fall(CUSTOM_DATA* data) {
	data->last_cum_dx = 0;
	data->last_cum_dy = 1;
	data->ticks = 0u;
	data->wait = WAIT_FALL;
	SetSpriteAnim(THIS, anim_slime_move, 5u);
}

static void slime_enter_rev(CUSTOM_DATA* data) {
	data->wait = WAIT_REV;
}

/* Turn away from a blocked direction and hop the other way if clear. */
static void slime_turn_and_go(CUSTOM_DATA* data) {
	slime_flip(data);
	slime_nudge_forward(data);
	if (slime_probe_ahead(data->dir) == 1u) {
		/* Both sides blocked — wait, try again later */
		slime_enter_idle(data);
	} else {
		slime_enter_hop(data);
	}
}

/* Void only — do not flip on water here (that caused shore idle loops). */
static UINT8 slime_check_void_fall(CUSTOM_DATA* data) {
	if (slime_support_below() == 2u) {
		slime_enter_fall(data);
		return 1u;
	}
	return 0u;
}

void START() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	if (IsMapEntityCleared(THIS) != 255) {
		SpriteManagerRemove(THIS_IDX);
		return;
	}
	memset(data, 0, sizeof(CUSTOM_DATA));
	data->wait = WAIT_IDLE;
	data->dir = 1;
	SetSpriteAnim(THIS, anim_slime_wait, 5u);
	THIS->lim_x = 500u;
	THIS->lim_y = 144u;
	if (player_sprite != NULL) {
		if (player_sprite->x < THIS->x) {
			THIS->mirror = V_MIRROR;
			data->dir = -1;
		} else {
			THIS->mirror = NO_MIRROR;
			data->dir = 1;
		}
	}
}

void UPDATE() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	UINT8 col;
	UINT8 probe;

	if (data->wait == WAIT_FALL) {
		if (data->last_cum_dy < (INT16)FALL_VY_MAX) data->last_cum_dy++;
		col = TranslateSpriteI16(THIS, 0, (INT16)(data->last_cum_dy << delta_time));
		if (col) {
			if (slime_is_water(col)) {
				/* Landed against water collision — face inland and leave */
				slime_turn_and_go(data);
			} else {
				slime_enter_idle(data);
			}
			return;
		}
		if (THIS->y > scroll_h) SpriteManagerRemove(THIS_IDX);
		return;
	}

	if (data->wait == WAIT_REV) {
		UINT16 t = data->ticks;
		if (t == 0u) {
			slime_turn_and_go(data);
			return;
		}
		{
			UINT16 t0 = t - 1u;
			INT16 cur_cum_dx = (INT16)(data->dir * slime_dx_table[t]);
			INT16 cur_cum_dy = (INT16)(-slime_dy_table[t]);
			INT16 next_cum_dx = (INT16)(data->dir * slime_dx_table[t0]);
			INT16 next_cum_dy = (INT16)(-slime_dy_table[t0]);
			TranslateSpriteI16(THIS,
				(INT16)((next_cum_dx - cur_cum_dx) << delta_time),
				(INT16)((next_cum_dy - cur_cum_dy) << delta_time));
			data->last_cum_dx = next_cum_dx;
			data->last_cum_dy = next_cum_dy;
			data->ticks = (UINT8)t0;
		}
		if (slime_check_void_fall(data)) return;
		return;
	}

	if (data->wait == WAIT_IDLE) {
		data->ticks++;
		if (data->ticks >= (UINT8)SLIME_DURATION) {
			probe = slime_probe_ahead(data->dir);
			if (probe == 1u) {
				/* Water / wall ahead — turn and hop the other way now */
				slime_turn_and_go(data);
				return;
			}
			slime_enter_hop(data);
			return;
		}
		if (slime_check_void_fall(data)) return;
		return;
	}

	/* ----- Hopping ----- */
	if (data->ticks < (UINT8)SLIME_DURATION) {
		UINT16 t = data->ticks;
		INT16 dx = slime_dx_table[t];
		INT16 dy = slime_dy_table[t];
		INT16 cur_cum_dx = (INT16)(data->dir * dx);
		INT16 cur_cum_dy = (INT16)(-dy);
		INT16 delta_x = cur_cum_dx - data->last_cum_dx;
		INT16 delta_y = cur_cum_dy - data->last_cum_dy;

		if (dy <= 2) {
			probe = slime_probe_ahead(data->dir);
			if (probe == 1u) {
				if (t == 0u) {
					slime_turn_and_go(data);
				} else {
					slime_enter_rev(data);
				}
				return;
			}
		}

		col = TranslateSpriteI16(THIS,
			(INT16)(delta_x << delta_time),
			(INT16)(delta_y << delta_time));

		if (col) {
			if (slime_is_water(col) || slime_is_floor(col)) {
				if (t == 0u) slime_turn_and_go(data);
				else slime_enter_rev(data);
				return;
			}
		}

		if (player_sprite != NULL && CheckCollision(THIS, player_sprite)) {
			if (t == 0u) slime_enter_idle(data);
			else slime_enter_rev(data);
			return;
		}

		data->last_cum_dx = cur_cum_dx;
		data->last_cum_dy = cur_cum_dy;
		data->ticks++;

		if (slime_check_void_fall(data)) return;
		return;
	}

	/* Finished hop arc */
	{
		INT16 final_dx = (INT16)(data->dir * MOVE_WIDTH) - data->last_cum_dx;
		INT16 final_dy = (INT16)(0) - data->last_cum_dy;
		if (final_dx || final_dy) {
			TranslateSpriteI16(THIS,
				(INT16)(final_dx << delta_time),
				(INT16)(final_dy << delta_time));
		}
	}
	if (slime_check_void_fall(data)) return;
	slime_enter_idle(data);
}

void DESTROY() {
}
