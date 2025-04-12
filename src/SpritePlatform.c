#include "Banks/SetAutoBank.h"
#include "main.h"
#include <gbdk/emu_debug.h>

#include <gb/gb.h>

#include "Scroll.h"
#include "SpriteManager.h"
#include "StateGame.h"
#include "SpritePlayer.h"

#define MAX_WAIT_TICKS	4

// player sprite pointer declaration, initialized in the SpritePlayer.c
extern Sprite *player_sprite;

const UINT8 anim_platform[] = {1, 0};

// Define the PlatformInfo structure
typedef struct PlatformInfo
{
	bool wait;
	UINT8 wait_ticks;
} PlatformInfo;

void START() {
	struct PlatformInfo* data = (struct PlatformInfo*)THIS->custom_data;
	data->wait = false;
	data->wait_ticks = 0;
	SetSpriteAnim(THIS, anim_platform, 20u);
	THIS->lim_x = 160u;
	THIS->lim_y = 160u;
	THIS->mirror = V_MIRROR;
}

UINT8 tile = 0;
void UPDATE() {
	struct PlatformInfo* data = (struct PlatformInfo*)THIS->custom_data;
	PlayerData* player_data = (PlayerData*)player_sprite->custom_data;
	if (!data->wait) {
		if (THIS->mirror == V_MIRROR) {
			//moving left
			//EMU_printf("SpritePlatform::UPDATE: moving left: %d:%d\n", (THIS->x), (THIS->y));
			tile = GetScrollTile((THIS->x - 4) >> 3, (THIS->y >> 3));
			//EMU_printf("SpritePlatform::UPDATE: moving left scroll tile: %d\n", tile);
			if (TranslateSprite(THIS, -1 << delta_time, 0)) {
				THIS->mirror = NO_MIRROR;
			}
			if (FLAG_CHECK(player_data->flags, pOnPlatformFlag) && (tile == TILE_INDEX_BG1 || tile == 255u)) { // where does 255 come from?
				TranslateSprite(player_sprite, -1, 0);
			}
		} else {
			//moving right
			//EMU_printf("SpritePlatform::UPDATE: moving right: %d:%d\n", (THIS->x), (THIS->y));
			tile = GetScrollTile((THIS->x + THIS->coll_w + 4) >> 3, (THIS->y >> 3));
			//EMU_printf("SpritePlatform::UPDATE: moving right scroll tile: %d\n", tile);
			if (TranslateSprite(THIS, +1 << delta_time, 0)) {
				THIS->mirror = V_MIRROR;
			}
			if (FLAG_CHECK(player_data->flags, pOnPlatformFlag) && (tile == TILE_INDEX_BG1 || tile == 255u)) { // where does 255 come from?
				TranslateSprite(player_sprite, +1, 0);
			}
		}
		if (tile != TILE_INDEX_BG1 && tile != 255u) data->wait = true;
	} else {
		if (data->wait_ticks++ == MAX_WAIT_TICKS) {
			data->wait = false;
			data->wait_ticks = 0;
		}
    }	
}

void DESTROY() {
}