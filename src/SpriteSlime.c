#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>

#include "Scroll.h"
#include "SpriteManager.h"

const UINT8 anim_slime_move[] = {9, 0, 1, 1, 1, 1, 1, 2, 1, 3};
struct EnemyInfo
{
    UINT8 wait;
};

void START() {
	struct EnemyInfo* data = (struct EnemyInfo*)THIS->custom_data;
	data->wait = 0;
	SetSpriteAnim(THIS, anim_slime_move, 5u);
	//THIS->lim_x = 160u;
	//THIS->lim_y = 160u;
}

void UPDATE() {
	struct EnemyInfo* data = (struct EnemyInfo*)THIS->custom_data;
	if (THIS->anim_frame > 1 && THIS->anim_frame < 7) return;
	if (!data->wait) {
		if (THIS->mirror == V_MIRROR) {
			//moving left
			if (TranslateSprite(THIS, -1 << delta_time, 0)) {
				THIS->mirror = V_MIRROR;
			} else if (!scroll_collisions[GetScrollTile((THIS->x >> 3) - 2u, (THIS->y >> 3) + 2u)]) {
				THIS->mirror = NO_MIRROR;
			}
		} else {
			//moving right
			if (TranslateSprite(THIS, +1 << delta_time, 0)) {
				THIS->mirror = NO_MIRROR;
			} else if (!scroll_collisions[GetScrollTile(((THIS->x + THIS->coll_w) >> 3), (THIS->y >> 3) + 2u)]) {
				THIS->mirror = V_MIRROR;
			}
		}
		data->wait = 1;
    } else {
		if (data->wait++ == 2) data->wait = 0;
    }
}

void DESTROY() {
}