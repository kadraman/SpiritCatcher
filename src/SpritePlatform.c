#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>

#include "Scroll.h"
#include "SpriteManager.h"

const UINT8 anim_platform[] = {1, 0};
struct PlatformInfo
{
    BYTE wait;
};

void START() {
	struct PlatformInfo* data = (struct PlatformInfo*)THIS->custom_data;
	data->wait = 0;
	SetSpriteAnim(THIS, anim_platform, 20u);
	THIS->lim_x = 160u;
	THIS->lim_y = 160u;
}

void UPDATE() {
	struct PlatformInfo* data = (struct PlatformInfo*)THIS->custom_data;
	if (THIS->mirror == V_MIRROR) {
		//moving left
		if (TranslateSprite(THIS, -1, 0)) {
			THIS->mirror = V_MIRROR;
		} else if (!scroll_collisions[GetScrollTile((THIS->x >> 3) - 2u, (THIS->y >> 3))]) {
			THIS->mirror = NO_MIRROR;
		}
	} else {
		//moving right
		if (TranslateSprite(THIS, +1, 0)) {
			THIS->mirror = V_MIRROR;
		} else if (!scroll_collisions[GetScrollTile(((THIS->x + THIS->coll_w) >> 3), (THIS->y >> 3))]) {
			THIS->mirror = V_MIRROR;
		}
	}
}

void DESTROY() {
}