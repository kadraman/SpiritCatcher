#include "Banks/SetAutoBank.h"
#include "main.h"
#include <gbdk/emu_debug.h>

#include <gb/gb.h>

#include "Scroll.h"
#include "SpriteManager.h"

#include "SpritePlayer.h"

// player sprite pointer declaration, initialized in the SpritePlayer.c
extern Sprite *player_sprite;

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
	PlayerData* player_data = (PlayerData*)player_sprite->custom_data;
	if (!data->wait) {
		if (THIS->mirror == V_MIRROR) {
			//moving left
			//EMU_printf("SpritePlatform::UPDATE: moving left: %d:%d\n", (THIS->x >> 3), (THIS->y >> 3));
			//UINT8 tile = GetScrollTile((THIS->x >> 3) - 2u, (THIS->y >> 3));
			//EMU_printf("SpritePlatform::UPDATE: scroll tile: %d\n", tile);
			if (TranslateSprite(THIS, -1, 0)) {
				THIS->mirror = NO_MIRROR;
			}
			if (FLAG_CHECK(player_data->flags, pOnPlatformFlag)) {
				TranslateSprite(player_sprite, -1, 0);
			}
		} else {
			//moving right
			//EMU_printf("SpritePlatform::UPDATE: moving right: %d:%d\n", ((THIS->x + THIS->coll_w) >> 3), (THIS->y >> 3));
			//UINT8 tile = GetScrollTile(((THIS->x + THIS->coll_w) >> 3), (THIS->y >> 3));
			//EMU_printf("SpritePlatform::UPDATE: scroll tile: %d\n", tile);
			if (TranslateSprite(THIS, +1, 0)) {
				THIS->mirror = V_MIRROR;
			}
			if (FLAG_CHECK(player_data->flags, pOnPlatformFlag)) {
				TranslateSprite(player_sprite, +1, 0);
			}
		}
		data->wait = 1;
	} else {
		if (data->wait++ == 2) data->wait = 0;
    }	
}

void DESTROY() {
}