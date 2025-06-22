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
typedef struct
{
	bool wait;
	UINT8 wait_ticks;
} CUSTOM_DATA;
CHECK_CUSTOM_DATA_SIZE(CUSTOM_DATA); // check that CUSTOM_DATA struct fits the CUSTOM_DATA_SIZE space

void START() {
    CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;
	data->wait = false;
	data->wait_ticks = 0;
	SetSpriteAnim(THIS, anim_platform, 20u);
	THIS->lim_x = 64u;
	THIS->lim_y = 0u;
	THIS->mirror = V_MIRROR;
}

UINT8 tile = 0;
void UPDATE() {
    CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;
    PlayerData* player_data = (PlayerData*)player_sprite->custom_data;

    INT16 prev_x = THIS->x; // Store platform's previous X position

    if (!data->wait) {
        if (THIS->mirror == V_MIRROR) {
            tile = GetScrollTile((THIS->x - 4) >> 3, (THIS->y >> 3));
            if (TranslateSprite(THIS, -1 << delta_time, 0)) {
                THIS->mirror = NO_MIRROR;
            }
        } else {
            tile = GetScrollTile((THIS->x + THIS->coll_w + 4) >> 3, (THIS->y >> 3));
            if (TranslateSprite(THIS, +1 << delta_time, 0)) {
                THIS->mirror = V_MIRROR;
            }
        }
        if (tile != TILE_INDEX_BG1 && tile != 255u) data->wait = true;
    } else {
        if (data->wait_ticks++ >= MAX_WAIT_TICKS) {
            data->wait = false;
            data->wait_ticks = 0;
        }
    }

    // Calculate how much the platform moved this frame
    INT16 delta_x = THIS->x - prev_x;

    // If the player is on the platform, move the player by the same amount
    if (FLAG_CHECK(player_data->flags, pOnPlatformFlag)) {
        player_sprite->x += delta_x;
		// Is the player still on the platform
        if (!PlayerIsOnTopOfPlatform(player_sprite, THIS)) {
            FLAG_CLEAR(player_data->flags, pOnPlatformFlag);
        }
    }
}

void DESTROY() {
}