#include <stdbool.h>
#include <stdlib.h>
#include <gb/gb.h>
#include <gbdk/emu_debug.h>

#include "Banks/SetAutoBank.h"
#include "main.h"
#include "Keys.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "GameTypes.h"
#include "ZGBMain.h"


// player sprite pointer declaration, initialized in the SpritePlayer.c
extern Sprite *overplayer_sprite;

#define DEFAULT_ANIM_SPEED 8

const UINT8 anim_open[] = {3, 0, 1, 2};
const UINT8 anim_closed[] = {1, 3};
typedef struct {
    bool is_open; // true if the portal is open
    UINT8 level;  // level number this portal leads to
    UINT8 entry_tile_x; // x coordinate of the entry tile in the level map
    UINT8 entry_tile_y; // y coordinate of the entry tile in the level map
    UINT8 exit_tile_x;  // x coordinate of the exit tile in the level map
    UINT8 exit_tile_y;  // y coordinate of the exit tile in the level map
} CUSTOM_DATA;
ASSERT_CUSTOM_DATA_SIZE(CUSTOM_DATA, 8);

void AddPortal(UINT16 x, UINT16 y, bool is_open, UINT8 level, UINT8 entry_tile_x, UINT8 entry_tile_y, UINT8 exit_tile_x, UINT8 exit_tile_y) BANKED {
    Sprite* spr = SpriteManagerAdd(SpriteOverPortal, x, y);
    CUSTOM_DATA* data = (CUSTOM_DATA*)spr->custom_data;

    data->is_open = is_open;
    data->level = level;
    data->entry_tile_x = entry_tile_x;
    data->entry_tile_y = entry_tile_y;
    data->exit_tile_x = exit_tile_x;
    data->exit_tile_y = exit_tile_y;

    if (is_open) {
        SetSpriteAnim(spr, anim_open, DEFAULT_ANIM_SPEED);
    } else {
        SetSpriteAnim(spr, anim_closed, DEFAULT_ANIM_SPEED);
    }
}

void START() {
    CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;
    memset(data, 0, sizeof(CUSTOM_DATA));
    SetSpriteAnim(THIS, anim_closed, DEFAULT_ANIM_SPEED);
}

void UPDATE() {
    CUSTOM_DATA * data = (CUSTOM_DATA*)THIS->custom_data;

    if (data->is_open) {
        SetSpriteAnim(THIS, anim_open, DEFAULT_ANIM_SPEED);
        // If the portal is open, check for player collision
        if (CheckCollision(THIS, overplayer_sprite)) {
            // If the player collides with the portal, change the level
            EMU_printf("SpriteOverPortal::UPDATE: Player entered portal to level %d\n", data->level);
            //ChangeLevel(data->level, data->entry_tile, data->exit_tile);
            SetState(StateGame); // Example action, change to your level change logic
        }
    } else {
        // If the portal is not open, do nothing or show a message
        //EMU_printf("SpriteOverPortal::UPDATE: Portal is closed\n");
    }
}

void DESTROY() {
    // Optionally handle sprite destruction
}