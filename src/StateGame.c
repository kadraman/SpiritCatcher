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

#define BANKED_MAP(MAP, X, Y, SECONDS) {BANK(MAP), &MAP, X, Y, SECONDS}
#define LEVELS_END {0, 0, 0, 0}

struct MapInfoBanked {
	UINT8 bank;
	struct MapInfo* map;
	UINT16 start_x;
	UINT16 start_y;
	UINT8  seconds;
};

const struct MapInfoBanked levels[] = {
	BANKED_MAP(l1, 2, 104, 120),

	LEVELS_END
};

UINT8 collision_tiles[] = {
	TILE_INDEX_WATER_1, TILE_INDEX_WATER_2,
	TILE_INDEX_SPIKE_UP, TILE_INDEX_SPIKE_DOWN,
	65, 66, 67, 68, 69, 70,
	71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
	81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
	91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
	0
};
UINT8 collision_tiles_down[] = { 
	27, 0
};
UINT8 fastest_times[] = { 120 };
UINT8 level_complete;

#define MAX_COLLECTABLES 10
UINT16 collectables_taken[MAX_COLLECTABLES + 1];

void pause(UINT16 time) BANKED {
	if (time) while (time--) vsync();	
}

void START() {
	LOAD_SGB_BORDER(bg_border);
	const struct MapInfoBanked* level = &levels[g_level_current-1];
	scroll_top_movement_limit = 72;
	scroll_bottom_movement_limit = 110;
	level_max_time = level->seconds;
	level_complete = 0;

	scroll_target = SpriteManagerAdd(SpritePlayer, level->start_x, level->start_y);
	InitScroll(level->bank, level->map, collision_tiles, collision_tiles_down);

	memset(collectables_taken, 0, sizeof(collectables_taken));

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
	PlayerData* player_data = (PlayerData*)player_sprite->custom_data;
	switch (itype) {
		case ITEM_COIN:
			PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
			player_data->coins++;
			break;
		case ITEM_SPIRIT:
			PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x83, 0xA3, 0x87);
			FLAG_SET(player_data->flags, pHasSpiritFlag);
			break;
		case ITEM_PORTAL:
			break;
		default:
			break;
	}
	Hud_Update();
}

