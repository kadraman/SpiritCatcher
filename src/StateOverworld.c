#include "Banks/SetAutoBank.h"
#include "main.h"

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
} PortalStartPos;

#define NUM_PORTALS 3
const PortalStartPos portal_start_positions[NUM_PORTALS+1] = {
    { 5, 11 },   	// player start position

    { 10, 8 },   	// Portal 1 start position 10, 14
    { 23, 10 },   	// Portal 2 start position: 16, 9
	{ 30, 6 }   	// Portal 3 start position: 7, 4
};
static Sprite* portal_sprites[NUM_PORTALS+1] = { NULL };

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
	HIDE_HUD;
	PortalStartPos pos = portal_start_positions[g_next_portal-1];
	scroll_target = SpriteManagerAdd(SpriteOverPlayer, pos.x << 3, pos.y << 3);
	InitScroll(BANK(overworld), &overworld, overworld_collision_tiles, 0);

	// Add all portals and enable only the one for g_next_portal
    /*for (UINT8 i = 1; i <= NUM_PORTALS; ++i) {
		AddPortal(
			portal_start_positions[i].x << 3, 
			portal_start_positions[i].y << 3, 
			(i == g_next_portal), // is_open
			i, // level
			0, // entry_tile_x (not used in this example)
			0, // entry_tile_y (not used in this example)
			0, // exit_tile_x (not used in this example)
			0  // exit_tile_y (not used in this example)
		);
        //Sprite* portal = SpriteManagerAdd(SpriteOverPortal, portal_start_positions[i].x << 3, portal_start_positions[i].y << 3);
        //CUSTOM_DATA* pdata = (CUSTOM_DATA*)portal->custom_data;
        //pdata->is_open = (i == g_next_portal) ? true : false;
        //pdata->level = i;
        // Set other portal data as needed
    }*/
	
	//PlayMusic(timeup, 0);
}

void UPDATE() {
	/*if (ANY_KEY_PRESSED) {
		SetState(StateGame);
	}*/
	
    for (UINT8 i = 1; i <= NUM_PORTALS; ++i) {
        UINT16 portal_x = portal_start_positions[i].x << 3;
        UINT16 portal_y = portal_start_positions[i].y << 3;

        if (portal_x >= scroll_x && portal_x < scroll_x + SCREEN_WIDTH &&
            portal_y >= scroll_y && portal_y < scroll_y + SCREEN_HEIGHT) {
            if (portal_sprites[i] == NULL) {
                portal_sprites[i] = AddPortal(
                    portal_x,
                    portal_y,
                    (i == g_next_portal), // is_open
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
    }
	Stars_Animate();
}