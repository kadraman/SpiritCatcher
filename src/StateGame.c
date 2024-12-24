#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "Print.h"
#include "Sound.h"
#include "Keys.h"
#include "SGB.h"

#include "StateGame.h"
#include "SpritePlayer.h"
#include "GlobalVars.h"

UINT8 g_level_current = 1;
UINT16 g_level_coins = 0;
UINT16 g_level_spirits = 0;
//UINT16 g_level_bullets = 6;
UINT8 start_x, start_y;
extern UINT16 collectables_taken[];
extern Sprite* player_sprite;
extern UINT16 g_player_score;
extern UINT16 level_max_time;

IMPORT_MAP(gb_border);
IMPORT_MAP(l1);
//IMPORT_MAP(level01);
//IMPORT_MAP(level02);
//...
IMPORT_FONT(font);
//DECLARE_MUSIC(level1);

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
	87,  88,  89,  90,  91,  92,  93,  94,  95,  96, 97,
	98,  99,  100, 101, 102, 103, 104, 105, 106, 107,
	108, 109, 110, 111, 112, 113, 114, 115, 116, 117,
	118, 119, 120, 121, 122, 
	0
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
				//num_clients++;
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
	//static UINT8 start_x, start_y;
	const struct MapInfoBanked* level = &levels[g_level_current-1];
	memset(collectables_taken, 0, sizeof(collectables_taken));
	scroll_top_movement_limit = 72;
	scroll_bottom_movement_limit = 110;
	level_max_time = level->seconds;
	level_complete = 0;

	LocateStuff(level->bank, level->map, &start_x, &start_y);
	scroll_target = SpriteManagerAdd(SpritePlayer, start_x << 3, (start_y - 1) << 3);
	InitScroll(level->bank, level->map, collision_tiles, 0);

	Hud_Init();

	//INIT_SOUND();
	//PlayMusic(level1, 1);
	//HIDE_HUD;
	//InitScroll(BANK(titlescreen), &titlescreen, 0, 0);
	//gbt_stop();
	//NR52_REG = 0x80; //Enables sound, you should always setup this first
	//NR51_REG = 0xFF; //Enables all channels (left and right)
	//NR50_REG = 0x77; //Max volume
	//PlayMusic(titles, 0);
	
	//SetState(g_level_current+1);
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
		//case ITEM_BULLET:
		//	g_level_bullets++;
		//	break;
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

