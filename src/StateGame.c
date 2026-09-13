#include <stdbool.h>
#include <string.h>
#include <gbdk/emu_debug.h>

#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "SpriteManager.h"
#include "Print.h"
#include "Sound.h"
#include "Music.h"
#include "Keys.h"
#include "SGB.h"
#include "ZGBMain.h"

#include "GameTypes.h"
#include "StateGame.h"
#include "Hud.h"
#include "SpritePlayer.h"
#include "Water.h"


UINT8 start_x, start_y;
INT16 min_x, max_x, min_y, max_y;
bool g_level_complete;
bool g_player_dead;

extern Sprite* player_sprite;
extern UINT16 level_max_time;
UINT16 level_width;
UINT16 level_height;

IMPORT_MAP(gb_border);
IMPORT_MAP(l1);
//..
//IMPORT_FONT(font);

DECLARE_MUSIC(cognition);

#define BANKED_MAP(MAP, X, Y, SPIRITS, SECONDS, ENTITIES) {BANK(MAP), &MAP, X, Y, SPIRITS, SECONDS, ENTITIES}
#define LEVELS_END {0, 0, 0, 0, 0, 0}

struct MapInfoBanked {
	UINT8 bank;
	struct MapInfo* map;
	UINT16 start_x;			// player starting location
	UINT16 start_y;			// player starting location
	UINT8 spirits;			// number of spirits to collect
	UINT8 seconds;			// max time to complete level
	UINT8 entities;			// clearable map markers (must be <= MAX_CLEARED_ENTITIES)
};

const struct MapInfoBanked levels[] = {
	/* entities = count of GetTileReplacement tiles (255-tile < N_SPRITE_TYPES) on the map */
	BANKED_MAP(l1, 2, 104, 3, 240, 17),
	BANKED_MAP(l1, 2, 104, 3, 240, 17),
	// Add more levels here
	LEVELS_END
};

UINT8 collision_tiles[] = {
	TILE_INDEX_WATER_1, TILE_INDEX_WATER_2, TILE_INDEX_WATER_3,
	71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
	81,
	0
};
UINT8 collision_tiles_down[] = { 
	TILE_INDEX_LADDER_LEFT, TILE_INDEX_LADDER_RIGHT, 0
};
UINT8 fastest_times[] = { 120 };

/* [0] = count; [1..] = unique_ids cleared this visit (items + defeated enemies). */
UINT16 cleared_entities[MAX_CLEARED_ENTITIES + 1];

void pause(UINT16 time) BANKED {
	if (time) while (time--) vsync();	
}

static void AssertClearedEntitiesCapacity(UINT8 entity_count) {
	if (entity_count <= MAX_CLEARED_ENTITIES)
		return;
	EMU_printf(
		"StateGame: map has %u clearable entities but MAX_CLEARED_ENTITIES=%u — raise the limit\n",
		(unsigned)entity_count,
		(unsigned)MAX_CLEARED_ENTITIES
	);
#ifndef NDEBUG
	/* Halt in Debug so the overflow is obvious in Emulicious. */
	while (1) vsync();
#endif
}

UINT8 IsMapEntityCleared(Sprite* spr) BANKED {
	UINT8 i;
	for (i = 1; i != cleared_entities[0] + 1; ++i) {
		if (cleared_entities[i] == spr->unique_id)
			return i;
	}
	return 255;
}

void ClearMapEntity(Sprite* spr) BANKED {
	if (IsMapEntityCleared(spr) != 255)
		return;
	if (cleared_entities[0] >= MAX_CLEARED_ENTITIES) {
		EMU_printf(
			"StateGame: cleared_entities full (%u) — unique_id 0x%04X not recorded\n",
			(unsigned)MAX_CLEARED_ENTITIES,
			(unsigned)spr->unique_id
		);
		return;
	}
	cleared_entities[++cleared_entities[0]] = spr->unique_id;
}

UINT8 IsCollected(Sprite* collectable) BANKED {
	return IsMapEntityCleared(collectable);
}

void TakeCollectable(Sprite* collectable, ItemType itype) BANKED {
	ClearMapEntity(collectable);
	PlayerData* player_data = (PlayerData*)player_sprite->custom_data;
	switch (itype) {
		case ITEM_MANA:
			PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
			player_data->magix++;
			Hud_Update();
			break;
		case ITEM_HEALTH:
			PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
			if (g_player_lives < MAX_LIVES && g_player_lives < UCHAR_MAX) {
				g_player_lives++;
			}
			Hud_Update();
			break;
		case ITEM_SPIRIT:
			PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
			player_data->spirits = player_data->spirits > 0 ? player_data->spirits - 1 : 0;
			if (player_data->spirits == 0) {
				FLAG_SET(player_data->flags, pCaughtSpiritFlag);
			}
			Hud_Update();
			break;
		case ITEM_PORTAL:
			break;
		default:
			break;
	}
	Hud_Update();
}

void START() {
	const struct MapInfoBanked* level = &levels[g_level_current-1];
	scroll_top_movement_limit = 72;
	scroll_bottom_movement_limit = 110;
	level_max_time = level->seconds;
	g_level_complete = false;
	g_player_dead = false;
	min_x = min_y = 1;
	scroll_target = SpriteManagerAdd(SpritePlayer, level->start_x, level->start_y);
	PlayerData* data = (PlayerData*)player_sprite->custom_data;
	data->spirits = level->spirits;
	InitScroll(level->bank, level->map, collision_tiles, collision_tiles_down);
	/* Use scroll_* set by InitScroll — map->width/height live in another bank. */
	level_width = scroll_w;
	level_height = scroll_h;

	memset(cleared_entities, 0, sizeof(cleared_entities));
	/* Capacity comes from levels[].entities — avoid SWITCH_ROM map scans in START. */
	AssertClearedEntitiesCapacity(level->entities);

	Hud_Init();

	INIT_SOUND();
	//PlayMusic(cognition, 1);

}

void UPDATE() {
	PlayerData* data = (PlayerData*)player_sprite->custom_data;
	if (g_player_dead) {
		//EMU_printf("StateGame::UPDATE: player is dead\n");
		HIDE_HUD;
		SpriteManagerRemoveSprite(player_sprite);
		SpriteManagerFlushRemove();
		player_sprite = NULL;
		SetState(StateGameOver);
	}
	if (g_level_complete) {
		g_level_current++;
		if (levels[g_level_current-1].map == 0) {
			SetState(StateWin);
			HIDE_HUD;
		} else {
			SetState(StateOverworld);
		}
	} else {
		Hud_Update();
		Water_Animate();
	}
}
