#include "Banks/SetAutoBank.h"
#include "main.h"
#include <gbdk/emu_debug.h>

#include <gb/gb.h>

#include "Scroll.h"
#include "SpriteManager.h"

const UINT8 anim_platform[] = {1, 0};
struct PlatformInfo
{
    UINT8 wait;
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
	if (!data->wait) {
		if (THIS->mirror == V_MIRROR) {
			//moving left
			//EMU_printf("SpritePlatform::UPDATE: moving left: %d:%d\n", (THIS->x >> 3), (THIS->y >> 3));
			//UINT8 tile = GetScrollTile((THIS->x >> 3) - 2u, (THIS->y >> 3));
			//EMU_printf("SpritePlatform::UPDATE: scroll tile: %d\n", tile);
			if (TranslateSprite(THIS, -1, 0)) {
				THIS->mirror = NO_MIRROR;
			}
		} else {
			//moving right
			//EMU_printf("SpritePlatform::UPDATE: moving right: %d:%d\n", ((THIS->x + THIS->coll_w) >> 3), (THIS->y >> 3));
			//UINT8 tile = GetScrollTile(((THIS->x + THIS->coll_w) >> 3), (THIS->y >> 3));
			//EMU_printf("SpritePlatform::UPDATE: scroll tile: %d\n", tile);
			if (TranslateSprite(THIS, +1, 0)) {
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