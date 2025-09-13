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
Sprite** portal_sprites = NULL;

IMPORT_MAP(overworld);
//DECLARE_MUSIC(overworld);

typedef struct {
    UINT8 x;
    UINT8 y;
    bool is_open;
} PortalStartPos;

PortalStartPos* portal_start_positions = NULL;
UINT8 num_portals = 0;
UINT8 player_start_x = 0;
UINT8 player_start_y = 0;

#define DEFAULT_OVERPLAYER_TILE_X 3
#define DEFAULT_OVERPLAYER_TILE_Y 8

#define HUD_HEIGHT_TILES 5
#define SCREEN_HEIGHT_TILES 18

#define HUD_TILE_BLANK 0
#define PORTAL_TILE_INDEX 307

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
    UINT8 found_portals = 0;

    // First, count portals
	PUSH_BANK(map_bank);
	data = map->data;
    EMU_printf("LocateStuff: map size %d x %d\n", map->width, map->height);
	for (y = 0; y < map->height; ++ y) {
		for (x = 0; x < map->width; ++ x) {
			tile = *(data ++);
            if (tile == 1) {
                // found player start position
                EMU_printf("LocateStuff: found player start at %d:%d\n", x, y);
                //set_bkg_tile_xy(x, y, 1);
                player_start_x = x;
                player_start_y = y;
            }
			if (tile > 1 && tile <= 10) {	// numbered portal
				EMU_printf("LocateStuff: found portal at %d:%d\n", x, y);
                found_portals++;
                //set_bkg_tile_xy(x, y, 1); // Replace with your desired tile index
                //UPDATE_BKG_TILE(x, y, 1);
			}
		}
	}
	POP_BANK;

    // Allocate array
    portal_start_positions = malloc(sizeof(PortalStartPos) * found_portals);
    num_portals = found_portals;
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
			if (tile > 1 && tile <= 10) {	// numbered portal
				EMU_printf("LocateStuff: found portal at %d:%d\n", x, y);
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
            tile = *(data++); idx=tile-1;
             if (tile == 1) {
                // found player start position
                //EMU_printf("LocateStuff: found player start at %d:%d\n", x, y);
                //set_bkg_tile_xy(x, y, 1);
                portal_start_positions[0].x = x;
                portal_start_positions[0].y = y;
                portal_start_positions[idx].is_open = false;
            }
            if (tile > 1 && tile <= 10) {    // numbered portal
                portal_start_positions[idx].x = x;
                portal_start_positions[idx].y = y;
                portal_start_positions[idx].is_open = (idx == g_level_current);
                EMU_printf("LocateStuff: portal %d at %d:%d, is_open=%d\n", idx, x, y, portal_start_positions[idx].is_open);
            }
        }
    }
    POP_BANK;
}

void START() {
    LocateStuff(BANK(overworld), &overworld);
    //for (UINT8 i = 0; i < num_portals; ++i) {
    //    EMU_printf("Portal %d at %d:%d, is_open=%d\n", i, portal_start_positions[i].x, portal_start_positions[i].y, portal_start_positions[i].is_open);
    //}
	PortalStartPos pos = portal_start_positions[g_level_current-1];
    if (g_level_current == 1) {
        player_start_x = pos.x;
        player_start_y = pos.y;
    } else {
        // Set position based on portal data
        player_start_x = portal_start_positions[g_level_current].x+1;   // TODO: use exit tile from portal data
        player_start_y = portal_start_positions[g_level_current].y;
    }
	scroll_target = SpriteManagerAdd(SpriteOverPlayer, player_start_x << 3, player_start_y << 3);
	InitScroll(BANK(overworld), &overworld, overworld_collision_tiles, 0);
    set_bkg_tile_xy(player_start_x, player_start_y, PORTAL_TILE_INDEX);
    set_bkg_attribute_xy(player_start_x, player_start_y, 2);                   
    
    Overworld_Hud_Init();

    INIT_SOUND();
	// Add all portals and enable only the one for g_level_current
    for (UINT8 i = 1; i <= num_portals; ++i) {
		AddPortal(
			portal_start_positions[i].x << 3, 
			portal_start_positions[i].y << 3, 
			(i == g_level_current), // is_open
			i, // level
			0, // entry_tile_x
			0, // entry_tile_y
			0, // exit_tile_x
			0  // exit_tile_y
		);
        EMU_printf("START: setting portal tile at %d:%d to tile ^%d\n", portal_start_positions[i].x, portal_start_positions[i].y, PORTAL_TILE_INDEX);
        //set_bkg_tile_xy(portal_start_positions[i].x, portal_start_positions[i].y, PORTAL_TILE_INDEX);
    }

    if (portal_sprites) free(portal_sprites);
    portal_sprites = malloc(sizeof(Sprite*) * num_portals);
    for (UINT8 i = 0; i < num_portals; ++i) portal_sprites[i] = NULL;
	
	//PlayMusic(overworld, 0);
}

void UPDATE() {
	
    for (UINT8 i = 1; i < num_portals; ++i) {
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
                set_bkg_tile_xy(portal_start_positions[i].x, portal_start_positions[i].y, PORTAL_TILE_INDEX);
                set_bkg_attribute_xy(portal_start_positions[i].x, portal_start_positions[i].y, 2);
            }
        } else {
            if (portal_sprites[i] != NULL) {
                SpriteManagerRemoveSprite(portal_sprites[i]);
                portal_sprites[i] = NULL;
            }
        }
    }

    Overworld_Hud_Update();
	Stars_Animate();
}

void DESTROY() {
	if (portal_start_positions) {
    	free(portal_start_positions);
    	portal_start_positions = NULL;
	}
    if (portal_sprites) {
        free(portal_sprites);
        portal_sprites = NULL;
    }
}