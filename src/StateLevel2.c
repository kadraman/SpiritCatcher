#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "Print.h"
#include "Sound.h"

#include "GlobalVars.h"
#include "SpritePlayer.h"
#include "Hud.h"

IMPORT_TILES(font);
IMPORT_MAP(level2);
IMPORT_MAP(hud);

UINT8 collision_tiles_l2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 
	TILE_ITEM_SPIKE, TILE_ITEM_FLAG, 0};

void START() {

	scroll_target = SpriteManagerAdd(SpritePlayer, 20, 80);
	InitScroll(BANK(level2), &level2, collision_tiles_l2, 0);
	scroll_bottom_movement_limit = 110;

	Hud_Init();

	NR52_REG = 0x80; //Enables sound, you should always setup this first
	NR51_REG = 0xFF; //Enables all channels (left and right)
	NR50_REG = 0x77; //Max volume

}

void UPDATE() {
	Hud_Update();
}

