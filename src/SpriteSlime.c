#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>

#include "Scroll.h"
#include "SpriteManager.h"

#include "SpriteSlime.h"
typedef struct {
	INT16 last_cum_dx;
	INT16 last_cum_dy;
	UINT16 ticks;
	INT8 dir; // +1 = right, -1 = left
	UINT8 wait;
} CUSTOM_DATA;
CHECK_CUSTOM_DATA_SIZE(CUSTOM_DATA);

void START() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	data->wait = 1;
	data->ticks = 0;
	data->last_cum_dx = 0;
	data->last_cum_dy = 0;
	data->dir = 1;
	SetSpriteAnim(THIS, anim_slime_wait, 5u);
	THIS->lim_x = 500u;
	THIS->lim_y = 144u;
	// Face the player at spawn
	if (player_sprite != NULL) {
		if (player_sprite->x < THIS->x) {
			THIS->mirror = V_MIRROR; // face left
		} else {
			THIS->mirror = NO_MIRROR; // face right
		}
	}
}

/*
 * Slime movement algorithm (curve driven by precomputed tables)
 * -----------------------------------------------------------
 * Overview:
 * - Movement is defined by two lookup tables: `slime_dx_table[]` and
 *   `slime_dy_table[]` (indexed 0..SLIME_DURATION). These tables contain
 *   cumulative offsets (in pixels) from the start of the movement for each
 *   frame `t` of the motion. Using cumulative values makes it cheap to
 *   compute per-frame deltas without runtime division.
 *
 * Forward motion (wait == 0):
 * - On each frame we read the cumulative offsets for the current progress
 *   `t`: dx = slime_dx_table[t], dy = slime_dy_table[t]. We apply the
 *   horizontal direction `dir` (+1 or -1) and invert Y as the engine uses
 *   screen coordinates where negative dy moves up.
 * - We compute the per-frame delta as (cur_cum - last_cum) and call
 *   `TranslateSprite(THIS, delta_x << delta_time, delta_y << delta_time)`.
 *   Using `TranslateSprite` preserves collision and scrolling semantics.
 * - If a tile collision occurs we enter reversing mode (wait == 2) so the
 *   slime will retrace the remaining curve frames back to the ground.
 * - If a sprite collision with the player occurs we also enter reversing
 *   mode so the slime smoothly returns to its start position.
 *
 * Reversing motion (wait == 2):
 * - To move backwards smoothly along the same parametric curve we sample
 *   the tables at `t` and `t-1` (or 0) and translate by the difference
 *   between those cumulative samples. This avoids big jumps and keeps the
 *   motion consistent with the forward traversal.
 * - When `t` reaches 0 the slime has completed the descent; we flip
 *   `dir`, set the idle state and reset cumulative trackers.
 *
 * Final notes:
 * - Precomputing cumulative offsets removes the need for runtime
 *   divisions or fixed-point reciprocal tricks on the Game Boy, reducing
 *   CPU cost to a table lookup and a small integer delta per frame.
 * - `delta_time` scaling is still applied to keep movement consistent
 *   across variable frame steps as per the engine convention.
 */
void UPDATE() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;

	// Reversing mode: step t down each frame following the original curve function
	if (data->wait == 2) {
		UINT16 t = data->ticks; // current progress
		if (t == 0) {
			// finished reversing: flip direction and go to idle
			data->last_cum_dx = 0;
			data->last_cum_dy = 0;
			data->ticks = 0u;
			data->wait = 1u;
			// flip direction after completing descent
			data->dir = (INT8)(-data->dir);
			THIS->mirror = (data->dir == -1) ? V_MIRROR : NO_MIRROR;
			SetSpriteAnim(THIS, anim_slime_wait, 5u);
			return;
		}

			const UINT16 DURATION = SLIME_DURATION;

			// compute cumulative at t and t-1 using lookup tables, then translate by their difference
			UINT16 t1 = t;
			UINT16 t0 = (t == 0) ? 0 : t - 1;

			INT16 dx1 = slime_dx_table[t1];
			INT16 dy1 = slime_dy_table[t1];

			INT16 dx0 = slime_dx_table[t0];
			INT16 dy0 = slime_dy_table[t0];

			INT16 cur_cum_dx = (INT16)(data->dir * dx1);
			INT16 cur_cum_dy = (INT16)(-dy1);
			INT16 next_cum_dx = (INT16)(data->dir * dx0);
			INT16 next_cum_dy = (INT16)(-dy0);

		INT16 delta_x = next_cum_dx - cur_cum_dx;
		INT16 delta_y = next_cum_dy - cur_cum_dy;

		TranslateSprite(THIS, delta_x << delta_time, delta_y << delta_time);

		data->last_cum_dx = next_cum_dx;
		data->last_cum_dy = next_cum_dy;
		data->ticks = t0;
		return;
	}

	const UINT16 DURATION = SLIME_DURATION; // frames for idle or move

	if (data->wait) {
		// idle: count until we start moving
		data->ticks++;
		if (data->ticks >= DURATION) {
			// start movement
			data->ticks = 0u;
			data->wait = 0u;
			data->dir = (THIS->mirror == V_MIRROR) ? -1 : 1;
			data->last_cum_dx = 0;
			data->last_cum_dy = 0;
			SetSpriteAnim(THIS, anim_slime_move, 5u);
		}
	} else if (data->wait == 0) {
		// moving along a single curve: horizontal 0..MOVE_WIDTH, vertical 0..MOVE_HEIGHT (peak at middle)
		if (data->ticks < DURATION) {
			// compute progress
			UINT16 t = data->ticks; // 0 .. DURATION-1
			// dx and dy from precomputed tables
			INT16 dx = slime_dx_table[t];
			// y = precomputed bump curve
			INT16 dy = slime_dy_table[t];

			// compute cumulative offsets (from start) and translate by the delta since last frame
			INT16 cur_cum_dx = (INT16)(data->dir * dx);
			INT16 cur_cum_dy = (INT16)(-dy);
			INT16 delta_x = cur_cum_dx - data->last_cum_dx;
			INT16 delta_y = cur_cum_dy - data->last_cum_dy;

			// translate using TranslateSprite so collisions/scroll are handled
			INT16 tx = (INT16)delta_x << delta_time;
			INT16 ty = (INT16)delta_y << delta_time;
			INT16 col = TranslateSprite(THIS, tx, ty);

			if (col) {
				// collision occurred: enter reversing mode to run remaining curve frames back to ground
				if (data->ticks == 0) {
					// already at ground (unlikely) -> just reset
					data->last_cum_dx = 0;
					data->last_cum_dy = 0;
					data->ticks = 0u;
					data->wait = 1u;
					SetSpriteAnim(THIS, anim_slime_wait, 5u);
				} else {
					// enter reversing mode; keep data->ticks as current progress t
					data->wait = 2; // reversing
					// do not flip dir yet; we'll flip after completing descent
				}
			} else {
				// if we hit the player (TranslateSprite doesn't detect sprite overlaps), handle bounce
				if (player_sprite != NULL && CheckCollision(THIS, player_sprite)) {
					// collided with player: enter reversing mode (run remaining frames back to ground)
					if (data->ticks == 0) {
						data->last_cum_dx = 0;
						data->last_cum_dy = 0;
						data->ticks = 0u;
						data->wait = 1u;
						SetSpriteAnim(THIS, anim_slime_wait, 5u);
					} else {
						data->wait = 2;
						// do not flip dir yet; flip after descent completes
					}
				} else {
					// store cumulative for next frame
					data->last_cum_dx = cur_cum_dx;
					data->last_cum_dy = cur_cum_dy;
					data->ticks++;
				}
			}

				// If we are in descent mode (returning to ground after collision), handle vertical return
				if (data->wait == 2) {
					// last_cum_dy is cumulative vertical offset (negative = up). Move it toward 0.
					if (data->last_cum_dy != 0) {
						INT16 step = (data->last_cum_dy < 0) ? 1 : -1; // move down if above ground
						TranslateSprite(THIS, 0, (INT16)step << delta_time);
						data->last_cum_dy += step;
					} else {
						// finished descent
						data->last_cum_dx = 0;
						data->last_cum_dy = 0;
						data->ticks = 0u;
						data->wait = 1u;
						SetSpriteAnim(THIS, anim_slime_wait, 5u);
					}
				}
		} else {
			// finished movement: place at final position and return to waiting
			// ensure any remaining delta is applied
			INT16 final_dx = (INT16)(data->dir * MOVE_WIDTH) - data->last_cum_dx;
			INT16 final_dy = (INT16)(0) - data->last_cum_dy;
			if (final_dx || final_dy) {
				INT16 col = TranslateSprite(THIS, (INT16)final_dx << delta_time, (INT16)final_dy << delta_time);
				if (col) {
					// collided with tile while applying final delta: reverse and go to waiting
					data->dir = (INT8)(-data->dir);
					THIS->mirror = (data->dir == -1) ? V_MIRROR : NO_MIRROR;
					// move back to start position
					INT16 back_x = (INT16)(-data->last_cum_dx);
					INT16 back_y = (INT16)(-data->last_cum_dy);
					if (back_x || back_y) {
						TranslateSprite(THIS, back_x << delta_time, back_y << delta_time);
					}
					data->last_cum_dx = 0;
					data->last_cum_dy = 0;
					data->ticks = 0u;
					data->wait = 1u;
					SetSpriteAnim(THIS, anim_slime_wait, 5u);
				} else if (player_sprite != NULL && CheckCollision(THIS, player_sprite)) {
					// collided with player on final placement: reverse and wait
					data->dir = (INT8)(-data->dir);
					THIS->mirror = (data->dir == -1) ? V_MIRROR : NO_MIRROR;
					INT16 back_x = (INT16)(-data->last_cum_dx);
					INT16 back_y = (INT16)(-data->last_cum_dy);
					if (back_x || back_y) {
						TranslateSprite(THIS, back_x << delta_time, back_y << delta_time);
					}
					data->last_cum_dx = 0;
					data->last_cum_dy = 0;
					data->ticks = 0u;
					data->wait = 1u;
					SetSpriteAnim(THIS, anim_slime_wait, 5u);
				}
			}
			data->ticks = 0u;
			data->wait = 1u;
			SetSpriteAnim(THIS, anim_slime_wait, 5u);
		}
	}
}

void DESTROY() {
}