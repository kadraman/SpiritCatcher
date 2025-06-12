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
//UINT8 g_player_lives; 
UINT8 g_player_region;
UINT8 start_x, start_y;
INT16 min_x, max_x, min_y, max_y;
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

#define BANKED_MAP(MAP, X, Y, SPIRITS, SECONDS) {BANK(MAP), &MAP, X, Y, SPIRITS, SECONDS}
#define LEVELS_END {0, 0, 0, 0, 0}

struct MapInfoBanked {
	UINT8 bank;
	struct MapInfo* map;
	UINT16 start_x;			// player starting location
	UINT16 start_y;			// player starting location
	UINT8 spirits;			// number of spirits to collect
	UINT8 seconds;			// max time to complete level
};

const struct MapInfoBanked levels[] = {
	BANKED_MAP(l1, 2, 104, 3, 240),
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

#define MAX_COLLECTABLES 10
UINT16 collectables_taken[MAX_COLLECTABLES + 1];

void pause(UINT16 time) BANKED {
	if (time) while (time--) vsync();	
}

/*void LocateStuff(UINT8 map_bank, struct MapInfo* map) __nonbanked{
	UINT8 x, y, tile;
	UINT8* data;
	PUSH_BANK(map_bank);
	data = map->data;
	for (y = 0; y < map->height; ++ y) {
		for (x = 0; x < map->width; ++ x) {
			tile = *(data ++);
			if (tile == 250) {	// spirit
				EMU_printf("LocateStuff: found spirit at %d:%d\n", x, y);
				num_spirits++;
			}
		}
	}
	POP_BANK;
}*/

void UpdateEnemies(UINT16 player_x, UINT16 player_y) BANKED {
	switch (g_level_current) {
		case 1:
			if (player_x > 0 && player_x <= 250 && g_player_region <= 0) {
				SpriteManagerAdd(SpriteSlime, 18 << 3, 13 << 3);
				SpriteManagerAdd(SpriteBat, 24 << 3, 9 << 3);
				g_player_region = 1; break;
			}
			if (player_x > 250 && player_x < 500 && g_player_region <= 1) {
				SpriteManagerAdd(SpriteSlime, 41 << 3, 13 << 3);
				SpriteManagerAdd(SpriteBat, 54 << 3, 10 << 3);
				g_player_region = 2; break;
			} 
			if (player_x > 500 && player_x < 750 && g_player_region <= 2) {
				SpriteManagerAdd(SpriteRockard, 79 << 3, 13 << 3);
				SpriteManagerAdd(SpriteBat, 69 << 3, 9 << 3);
				g_player_region = 3; break;
			}
			if (player_x > 750 && player_x < 1000 && g_player_region <= 3) {
				SpriteManagerAdd(SpriteBat, 95 << 3, 2 << 3);
				SpriteManagerAdd(SpriteSlime, 113 << 3, 4 << 3);
				g_player_region = 4; break;
			} 
			break;
		default:
			break;
	}
}

void START() {
	const struct MapInfoBanked* level = &levels[g_level_current-1];
	scroll_top_movement_limit = 72;
	scroll_bottom_movement_limit = 110;
	level_max_time = level->seconds;
	g_level_complete = false;
	//g_player_lives = MAX_LIVES;
	g_player_dead = false;
	g_player_region = 0;
	min_x = min_y = 1;
	//LocateStuff(level->bank, level->map);
	scroll_target = SpriteManagerAdd(SpritePlayer, level->start_x, level->start_y);
	PlayerData* data = (PlayerData*)player_sprite->custom_data;
	data->spirits = level->spirits;
	//data->lives = MAX_LIVES;
	InitScroll(level->bank, level->map, collision_tiles, collision_tiles_down);

	memset(collectables_taken, 0, sizeof(collectables_taken));

	Hud_Init();

	INIT_SOUND();
	//PlayMusic(cognition, 1);

}

void UPDATE() {
	PlayerData* data = (PlayerData*)player_sprite->custom_data;
	UpdateEnemies(player_sprite->x, player_sprite->y);
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
			SetState(StateGame);
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
			EMU_printf("SpritePlayer::%s: player now has %d magix\n", __func__, player_data->magix);
			Hud_Update();
			break;
		case ITEM_HEALTH:
			PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
			if (player_data->lives < MAX_LIVES && player_data->lives < UCHAR_MAX) {
				player_data->lives++;
			}
			EMU_printf("SpritePlayer::%s: player now has %d lives\n", __func__, player_data->lives);
			Hud_Update();
			break;
		case ITEM_SPIRIT:
			PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
			player_data->spirits--;
			if (player_data->spirits == 0) {
				EMU_printf("SpritePlayer::%s: player has all spirits\n", __func__);
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

