#include "Banks/SetAutoBank.h"
#include "main.h"
#include <stdlib.h>

#include <gb/gb.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Keys.h"
#include "Music.h"
#include "SpriteManager.h"
#include "GameTypes.h"
#include "Stars.h"
#include <BankManager.h>

//Sprite* AddPortal(UINT16 x, UINT16 y, bool is_open, UINT8 level, UINT8 entry_tile_x, UINT8 entry_tile_y, UINT8 exit_tile_x, UINT8 exit_tile_y) BANKED;
Sprite* EnablePortal(Sprite* spr) BANKED;
Sprite* DisablePortal(Sprite* spr) BANKED;

IMPORT_MAP(overworld);
//DECLARE_MUSIC(overworld);

typedef struct {
    UINT8 x;    // x coordinate in tiles
    UINT8 y;    // y coordinate in tiles
    UINT8 level; // level number this portal leads to
    bool is_open; // true if the portal is open
    bool is_complete; // true if the level has been completed
    UINT8 exit_tile_x;  // x coordinate of the exit tile in the level map
    UINT8 exit_tile_y;  // y coordinate of the exit tile in the level map
} PortalStatus;

UINT8 player_start_x = 0;
UINT8 player_start_y = 0;

#define DEFAULT_OVERPLAYER_TILE_X 3
#define DEFAULT_OVERPLAYER_TILE_Y 7

#define HUD_HEIGHT_TILES 5
#define SCREEN_HEIGHT_TILES 18

#define HUD_TILE_BLANK 0
#define PORTAL_TILE_INDEX 307


#define NUM_PORTALS 8
const PortalStatus portal_positions[NUM_PORTALS] = {
    { 7, 3, 1, false, false, 8, 3 },
    { 16, 3, 2, false, false, 17, 3 },
    { 25, 3, 3, false, false, 26, 3 },
    { 32, 7, 4, false, false, 31, 7 },
    { 11, 9, 5, false, false, 12, 9 },
    { 7, 12, 6, false, false, 8, 12 },
    { 27, 12, 7, false, false, 24, 12 },
    { 29, 10, 8, false, false, 30, 10 }
};

UINT8 overworld_collision_tiles[] = {
	58, 59, 60, 
	61, 62, 63,	64, 65, 66, 67, 68, 69, 70,
	71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
	81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
	91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
	101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
	111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
	121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
	131, 132, 133, 134, 135, 136, 137, 138, 139, 140
};

void START() {
    if (g_level_current == 1) {
        // Default start position for level 1
        player_start_x = DEFAULT_OVERPLAYER_TILE_X;
        player_start_y = DEFAULT_OVERPLAYER_TILE_Y;
    } else {
        // Set position based on portal data
        player_start_x = portal_positions[g_level_current-1].exit_tile_x;   
        player_start_y = portal_positions[g_level_current-1].exit_tile_y;
    }
	scroll_target = SpriteManagerAdd(SpriteOverPlayer, player_start_x << 3, player_start_y << 3);
	InitScroll(BANK(overworld), &overworld, overworld_collision_tiles, 0);                

    Overworld_Hud_Init();

    INIT_SOUND();

	//PlayMusic(overworld, 0);
}

void UPDATE() {
    UINT8 i;
    Sprite* spr;

    SPRITEMANAGER_ITERATE(i, spr) {
        //EMU_printf("UPDATE: Checking sprite %d of type %d at %d:%d\n", i, spr->type, spr->x, spr->y);
        if (spr->type == SpriteOverPortal) {
            //EMU_printf("UPDATE: Found portal sprite at %d:%d\n", spr->x, spr->y);
            UINT8 portal_tile_x = spr->x >> 3;
            UINT8 portal_tile_y = spr->y >> 3;

            // Check against all portal_positions
            for (UINT8 j = 0; j < NUM_PORTALS; j++) {
                //EMU_printf("UPDATE: Checking portal sprite at %d:%d against portal_positions[%d] at %d:%d\n", portal_tile_x, portal_tile_y, j, portal_positions[j].x, portal_positions[j].y);
                if (portal_tile_x == portal_positions[j].x && portal_tile_y == portal_positions[j].y) {
                    // Enable portal if needed
                    if (portal_positions[j].level == g_level_current) {
                        EnablePortal(spr);
                    } else {
                        DisablePortal(spr);
                    }
                }
            }
           
        }
    }

    Overworld_Hud_Update();
	Stars_Animate();
}

void DESTROY() {

}