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
#include "GlobalVars.h"

UINT8 g_level_current = 1;
UINT16 g_level_coins = 0;
UINT16 g_level_spirits = 0;
UINT8 start_x, start_y;
extern UINT16 collectables_taken[];
extern Sprite* player_sprite;
extern UINT16 g_player_score;
extern UINT16 level_max_time;

IMPORT_MAP(gb_border);
IMPORT_MAP(l1);
//..
//IMPORT_FONT(font);

DECLARE_MUSIC(cognition);

#define BANKED_MAP(MAP, SECONDS) {BANK(MAP), &MAP, SECONDS}
#define LEVELS_END {0, 0}

struct MapInfoBanked {
	UINT8 bank;
	struct MapInfo* map;
	UINT8 seconds;
};

const struct MapInfoBanked levels[] = {
	BANKED_MAP(l1, 120),

	LEVELS_END
};

UINT8 collision_tiles[] = {
	61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
	71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
	81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
	91, 92, 93, 94, 95, 96, 
	0
};
UINT8 collision_tiles_down[] = { 
	27, 0
};
UINT8 fastest_times[] = { 120 };

UINT8 level_complete;
UINT16 level_width;
UINT16 level_height;

#define MAX_COLLECTABLES 10
UINT16 collectables_taken[MAX_COLLECTABLES + 1];

void LocateStuff(UINT8 map_bank, struct MapInfo* map, UINT8* start_x, UINT8* start_y) NONBANKED {
	UINT8 * data, __save_bank = CURRENT_BANK;
	SWITCH_ROM(map_bank);
	level_width = map->width << 3;
	level_height = map->height << 3;
	data = map->data;
	for(UINT8 y = 0; y < map->height; ++ y) {
		for(UINT8 x = 0; x < map->width; ++ x) {
			UINT8 tile = *data++;
			if (tile == TILE_INDEX_COIN) {          //coins

			} else if (tile == TILE_INDEX_PLAYER) {  //player
				*start_x = x;
				*start_y = y;
			}
		}
	}
	SWITCH_ROM(__save_bank);
}

void START() {
	LOAD_SGB_BORDER(bg_border);
	const struct MapInfoBanked* level = &levels[g_level_current-1];
	memset(collectables_taken, 0, sizeof(collectables_taken));
	scroll_top_movement_limit = 72;
	scroll_bottom_movement_limit = 110;
	level_max_time = level->seconds;
	level_complete = 0;

	LocateStuff(level->bank, level->map, &start_x, &start_y);
	scroll_target = SpriteManagerAdd(SpritePlayer, start_x << 3, (start_y - 1) << 3);
	InitScroll(level->bank, level->map, collision_tiles, collision_tiles_down);

	Hud_Init();

	PlayMusic(cognition, 1);

}

void UPDATE() {
	if (!level_complete) {
		Hud_Update();
	} else {
		if (KEY_TICKED(J_START | J_A | J_B)) {
			g_level_current++;
			if (levels[g_level_current-1].map == 0)
				SetState(StateWin);
			else
				SetState(StateGame);
			HIDE_HUD;
		}
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
	PlayerData* data = (PlayerData*)player_sprite->custom_data;
	switch (itype) {
		case ITEM_COIN:
			g_level_coins++;
			break;
		case ITEM_SPIRIT:
			g_level_spirits++;
		default:
			break;
	}
	Hud_Update();
}

