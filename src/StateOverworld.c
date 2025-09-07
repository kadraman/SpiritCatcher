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

//UINT8 start_x;
//UINT8 start_y;

Sprite* AddPortal(UINT16 x, UINT16 y, bool is_open, UINT8 level, UINT8 entry_tile_x, UINT8 entry_tile_y, UINT8 exit_tile_x, UINT8 exit_tile_y) BANKED;

extern Sprite* overplayer_sprite;

IMPORT_MAP(overworld);
//DECLARE_MUSIC(timeup);

typedef struct {
    UINT8 x;
    UINT8 y;
    bool is_open;
} PortalStartPos;

PortalStartPos* portal_start_positions = NULL;
UINT8 num_portals = 0;

#define DEFAULT_OVERPLAYER_TILE_X 3
#define DEFAULT_OVERPLAYER_TILE_Y 8

#define HUD_HEIGHT_TILES 5
#define SCREEN_HEIGHT_TILES 18

#define HUD_TILE_BLANK 0

/*
#define NUM_PORTALS 3
const PortalStartPos portal_start_positions[NUM_PORTALS+1] = {
    { 5, 11 },   	// player start position

    { 10, 8 },   	// Portal 1 start position 10, 14
    { 23, 10 },   	// Portal 2 start position: 16, 9
	{ 30, 6 }   	// Portal 3 start position: 7, 4
};
static Sprite* portal_sprites[NUM_PORTALS+1] = { NULL };
*/

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

/*void LocateStuff(UINT8 map_bank, struct MapInfo* map) __nonbanked{
	UINT8 x, y, tile;
	UINT8* data;
	PUSH_BANK(map_bank);
	data = map->data;
	for (y = 0; y < map->height; ++ y) {
		for (x = 0; x < map->width; ++ x) {
			tile = *(data ++);
			if (tile == 255) {	// spirit
				EMU_printf("LocateStuff: found portal at %d:%d\n", x, y);
			}
		}
	}
	POP_BANK;
}*/

void LocateStuff(UINT8 map_bank, struct MapInfo* map) __nonbanked {
    UINT8 x, y, tile;
    UINT8* data;
    UINT8 found_portals = 0;

    // First, count portals
    PUSH_BANK(map_bank);
    data = map->data;
    for (y = 0; y < map->height; ++y) {
        for (x = 0; x < map->width; ++x) {
            tile = *(data++);
			if (tile == 255 && g_level_current) {
				// found player start position
			}
            if (tile == 254) {
                found_portals++;
            }
        }
    }
    POP_BANK;

    // Allocate array
    portal_start_positions = malloc(sizeof(PortalStartPos) * found_portals);
    num_portals = found_portals;

    // Fill array
    PUSH_BANK(map_bank);
    data = map->data;
    UINT8 idx = 0;
    for (y = 0; y < map->height; ++y) {
        for (x = 0; x < map->width; ++x) {
            tile = *(data++);
            if (tile == 255) {
                portal_start_positions[idx].x = x;
                portal_start_positions[idx].y = y;
                portal_start_positions[idx].is_open = (idx == g_level_current - 1);
                idx++;
            }
        }
    }
    POP_BANK;
}

void START() {
    LocateStuff(BANK(overworld), &overworld);
	PortalStartPos pos = portal_start_positions[g_level_current-1];
    if (g_level_current == 1) {
        pos.x = DEFAULT_OVERPLAYER_TILE_X;
        pos.y = DEFAULT_OVERPLAYER_TILE_Y;
    } else {
        // Set position based on portal data
        pos.x = portal_start_positions[g_level_current-1].x+1;
        pos.y = portal_start_positions[g_level_current-1].y;
    }
	scroll_target = SpriteManagerAdd(SpriteOverPlayer, pos.x << 3, pos.y << 3);
	InitScroll(BANK(overworld), &overworld, overworld_collision_tiles, 0);

    Overworld_Hud_Init();

    INIT_SOUND();
	// Add all portals and enable only the one for g_level_current
    /*for (UINT8 i = 1; i <= NUM_PORTALS; ++i) {
		AddPortal(
			portal_start_positions[i].x << 3, 
			portal_start_positions[i].y << 3, 
			(i == g_level_current), // is_open
			i, // level
			0, // entry_tile_x (not used in this example)
			0, // entry_tile_y (not used in this example)
			0, // exit_tile_x (not used in this example)
			0  // exit_tile_y (not used in this example)
		);
        //Sprite* portal = SpriteManagerAdd(SpriteOverPortal, portal_start_positions[i].x << 3, portal_start_positions[i].y << 3);
        //CUSTOM_DATA* pdata = (CUSTOM_DATA*)portal->custom_data;
        //pdata->is_open = (i == g_level_current) ? true : false;
        //pdata->level = i;
        // Set other portal data as needed
    }*/
	
	//PlayMusic(timeup, 0);
}

void UPDATE() {
	/*if (ANY_KEY_PRESSED) {
		SetState(StateGame);
	}*/
	
    /*
    for (UINT8 i = 1; i <= NUM_PORTALS; ++i) {
        UINT16 portal_x = portal_start_positions[i].x << 3;
        UINT16 portal_y = portal_start_positions[i].y << 3;

        if (portal_x >= scroll_x && portal_x < scroll_x + SCREEN_WIDTH &&
            portal_y >= scroll_y && portal_y < scroll_y + SCREEN_HEIGHT) {
            if (portal_sprites[i] == NULL) {
                portal_sprites[i] = AddPortal(
                    portal_x,
                    portal_y,
                    (i == g_level_current), // is_open
                    i, 0, 0, 0, 0
                );
            }
        } else {
            // Optionally remove portal if it leaves the screen
            if (portal_sprites[i] != NULL) {
                SpriteManagerRemoveSprite(portal_sprites[i]);
                portal_sprites[i] = NULL;
            }
        }
    }*/
    Overworld_Hud_Update();
	Stars_Animate();
}

void DESTROY() {
	if (portal_start_positions) {
    	free(portal_start_positions);
    	portal_start_positions = NULL;
	}
}