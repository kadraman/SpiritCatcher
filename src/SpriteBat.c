#include "Banks/SetAutoBank.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>

#include <gb/gb.h>

#include "Scroll.h"
#include "SpriteManager.h"
#include "ZGBMain.h"
#include "StateGame.h"

extern Sprite *player_sprite;

/* Tuned for a floaty chase — a bit snappier than the first slow pass. */
#define AGGRO_RANGE      56u  /* start facing/closing in */
#define MOVE_DIV_FAR      3u  /* frames between steps when far */
#define MOVE_DIV_NEAR     2u  /* frames between steps when near */
#define RETREAT_FRAMES   12u
#define COOLDOWN_FRAMES  32u  /* wait after a hit before chasing again */
#define BOB_DIV          10u
/*
 * Knife (SpriteAttack1) is beside the player at mid-body. Aim at torso height
 * so the bat crosses the slash zone on the way in — do NOT hold a horizontal
 * standoff (that made the bat flee when the player walked toward it).
 */
#define AIM_Y_OFF        10u  /* ~slash / torso height on 16px player */

#define STATE_CHASE    0u
#define STATE_RETREAT  1u
#define STATE_COOLDOWN 2u

const UINT8 anim_bat_move[] = {3, 0, 1, 2};
const UINT8 anim_bat_attack[] = {3, 3, 4, 5};

typedef struct {
	UINT8 state;
	UINT8 timer;     /* retreat / cooldown countdown */
	UINT8 pace;      /* move divider counter */
	UINT8 bob;
	INT8 rdx;
	INT8 rdy;
	UINT8 anim_attack; /* 0 = move anim, 1 = attack anim (avoid retrigger) */
	UINT8 pad;
} CUSTOM_DATA;
ASSERT_CUSTOM_DATA_SIZE(CUSTOM_DATA, 8);

static void bat_set_anim(Sprite* spr, CUSTOM_DATA* data, UINT8 attack) {
	if (data->anim_attack == attack) return;
	data->anim_attack = attack;
	if (attack) {
		SetSpriteAnim(spr, anim_bat_attack, 8u);
	} else {
		SetSpriteAnim(spr, anim_bat_move, 8u);
	}
}

static void bat_move_x(Sprite* spr, INT8 step) {
	if (step < 0) {
		if (spr->x > 0u) spr->x -= 1u;
	} else if (step > 0) {
		if (scroll_w > spr->coll_w && spr->x < scroll_w - spr->coll_w) spr->x += 1u;
	}
}

static void bat_move_y(Sprite* spr, INT8 step) {
	if (step < 0) {
		if (spr->y > 0u) spr->y -= 1u;
	} else if (step > 0) {
		if (scroll_h > spr->coll_h && spr->y < scroll_h - spr->coll_h) spr->y += 1u;
	}
}

void START() {
	CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;
	if (IsMapEntityCleared(THIS) != 255) {
		SpriteManagerRemove(THIS_IDX);
		return;
	}
	memset(data, 0, sizeof(CUSTOM_DATA));
	data->state = STATE_CHASE;
	data->anim_attack = 0xFFu; /* force first SetSpriteAnim */
	bat_set_anim(THIS, data, 0u);
	THIS->lim_x = 500;
	THIS->lim_y = 144;
}

void UPDATE() {
	CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;
	UINT16 pcx, pcy, bcx, bcy;
	UINT16 adx, ady;
	INT8 sx, sy;
	UINT8 near;
	UINT8 div;

	if (player_sprite == NULL) return;

	/* Aim at player torso (slash height), always chase toward them. */
	pcx = player_sprite->x + (player_sprite->coll_w >> 1);
	pcy = player_sprite->y + AIM_Y_OFF;
	bcx = THIS->x + (THIS->coll_w >> 1);
	bcy = THIS->y + (THIS->coll_h >> 1);

	adx = (pcx > bcx) ? (pcx - bcx) : (bcx - pcx);
	ady = (pcy > bcy) ? (pcy - bcy) : (bcy - pcy);
	near = (adx < AGGRO_RANGE && ady < AGGRO_RANGE) ? 1u : 0u;

	/* ----- Retreat: hop sideways into knife range, little vertical ----- */
	if (data->state == STATE_RETREAT) {
		bat_move_x(THIS, data->rdx);
		if ((data->timer & 1u) != 0u) bat_move_y(THIS, data->rdy);
		if (data->timer > 0u) data->timer--;
		if (data->timer == 0u) {
			data->state = STATE_COOLDOWN;
			data->timer = COOLDOWN_FRAMES;
			bat_set_anim(THIS, data, 0u);
		}
		return;
	}

	/* ----- Cooldown: hover; nudge toward slash height only ----- */
	if (data->state == STATE_COOLDOWN) {
		data->bob++;
		if ((data->bob % BOB_DIV) == 0u) {
			if (bcy + 1u < pcy) bat_move_y(THIS, 1);
			else if (bcy > pcy + 1u) bat_move_y(THIS, -1);
			else bat_move_y(THIS, (data->bob & 16u) ? (INT8)1 : (INT8)-1);
		}
		if (data->timer > 0u) data->timer--;
		if (data->timer == 0u) {
			data->state = STATE_CHASE;
		}
		return;
	}

	/* ----- Chase ----- */
	sx = 0;
	sy = 0;
	if (pcx > bcx) sx = 1;
	else if (pcx < bcx) sx = -1;
	if (pcy > bcy) sy = 1;
	else if (pcy < bcy) sy = -1;

	if (sx < 0) THIS->mirror = V_MIRROR;
	else if (sx > 0) THIS->mirror = NO_MIRROR;

	bat_set_anim(THIS, data, near);

	data->pace++;
	data->bob++;
	div = near ? MOVE_DIV_NEAR : MOVE_DIV_FAR;

	if ((data->pace % div) == 0u) {
		/* Match slash height first when close, then close distance */
		if (near && ady > 1u) {
			bat_move_y(THIS, sy);
		} else if (adx > ady) {
			bat_move_x(THIS, sx);
		} else if (ady > 0u) {
			bat_move_y(THIS, sy);
		} else {
			bat_move_x(THIS, sx);
		}
	}

	if (!near && (data->bob % BOB_DIV) == 0u) {
		bat_move_y(THIS, (data->bob & 16u) ? (INT8)1 : (INT8)-1);
	}

	if (CheckCollision(THIS, player_sprite)) {
		/* Sideways hop — bat stays near slash height beside the player */
		data->rdx = (bcx <= pcx) ? (INT8)-1 : (INT8)1;
		data->rdy = 0;
		data->timer = RETREAT_FRAMES;
		data->state = STATE_RETREAT;
		bat_set_anim(THIS, data, 0u);
	}
}

void DESTROY() {
}
