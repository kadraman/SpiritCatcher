#include <stdbool.h>
#include <gbdk/emu_debug.h>

#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "Print.h"
#include "Sound.h"
#include "Music.h"
#include "Keys.h"
#include "SGB.h"

#include "StateGame.h"
#include "SpritePlayer.h"
#include "Water.h"
#include "GameTypes.h"

UINT8 g_level_current = 1;
UINT8 g_player_lives; 
UINT8 start_x, start_y;
INT16 min_x, max_x, min_y, max_y;
UINT8 num_spirits = 0;
bool g_level_complete;
bool g_player_dead;

extern UINT16 collectables_taken[];
extern Sprite* player_sprite;
extern UINT16 level_max_time;

IMPORT_MAP(gb_border);
IMPORT_MAP(l1);
//..
//IMPORT_FONT(font);

DECLARE_MUSIC(cognition);

#define BANKED_MAP(MAP, X, Y, SECONDS) {BANK(MAP), &MAP, X, Y, SECONDS}
#define LEVELS_END {0, 0, 0, 0}

struct MapInfoBanked {
	UINT8 bank;
	struct MapInfo* map;
	UINT16 start_x;			// player starting location
	UINT16 start_y;			// player starting location
	UINT8  seconds;			// ax time to complete level
};

const struct MapInfoBanked levels[] = {
	BANKED_MAP(l1, 2, 104, 240),

	LEVELS_END
};

UINT8 collision_tiles[] = {
	TILE_INDEX_WATER_1, TILE_INDEX_WATER_2,
	TILE_INDEX_SPIKE_UP, TILE_INDEX_SPIKE_DOWN,
	72, 73, 74, 75, 76, 77, 78, 79, 80,
	81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
	91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
	101, 102, 103, 104, 105, 106, 107, 108,
	0
};
UINT8 collision_tiles_down[] = { 
	TILE_INDEX_LADDER_LEFT, TILE_INDEX_LADDER_RIGHT, 0
};
UINT8 fastest_times[] = { 120 };

#define MAX_COLLECTABLES 10
UINT16 collectables_taken[MAX_COLLECTABLES + 1];

void pause(UINT16 time) BANKED {
	if (time) while (time--) vsync();	
}

void LocateStuff(UINT8 map_bank, struct MapInfo* map) __nonbanked{
	UINT8 x, y, tile;
	UINT8* data;
	PUSH_BANK(map_bank);
	data = map->data;
	for (y = 0; y < map->height; ++ y) {
		for (x = 0; x < map->width; ++ x) {
			tile = *(data ++);
			if (tile == 251) {	// spirit
				EMU_printf("LocateStuff: found spirit at %d:%d\n", x, y);
				num_spirits ++;
			}
		}
	}
	POP_BANK;
}

void START() {
	const struct MapInfoBanked* level = &levels[g_level_current-1];
	scroll_top_movement_limit = 72;
	scroll_bottom_movement_limit = 110;
	level_max_time = level->seconds;
	g_level_complete = false;
	g_player_lives = MAX_LIVES;
	g_player_dead = false;
	min_x = min_y = 1;
	num_spirits = 0;
	LocateStuff(level->bank, level->map);
	EMU_printf("StateGame::%s: level %d, spirits %d\n", __func__, g_level_current, num_spirits);
	scroll_target = SpriteManagerAdd(SpritePlayer, level->start_x, level->start_y);
	PlayerData* data = (PlayerData*)player_sprite->custom_data;
	data->spirits = num_spirits;
	InitScroll(level->bank, level->map, collision_tiles, collision_tiles_down);

	memset(collectables_taken, 0, sizeof(collectables_taken));

	Hud_Init();

	PlayMusic(cognition, 1);

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

UINT8 IsCollected(Sprite* collectable) BANKED {
	UINT8 i;
	for (i = 1; i != collectables_taken[0] + 1; ++i) {
		if (collectables_taken[i] == collectable->unique_id)
			return i;
	}
	return 255;
}

void TakeCollectable(Sprite* collectable, ItemType itype) BANKED {
	collectables_taken[++ collectables_taken[0]] = collectable->unique_id;
	PlayerData* player_data = (PlayerData*)player_sprite->custom_data;
	switch (itype) {
		case ITEM_MANA:
			PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
			player_data->magix++;
			break;
		//case ITEM_COIN:
		//	PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
		//	player_data->coins++;
		//	break;
		case ITEM_SPIRIT:
			PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
			player_data->spirits--;
			if (player_data->spirits == 0) {
				EMU_printf("SpritePlayer::%s: player has all spirits\n", __func__);
				FLAG_SET(player_data->flags, pCaughtSpiritFlag);
			}
			break;
		case ITEM_PORTAL:
			break;
		default:
			break;
	}
	Hud_Update();
}

