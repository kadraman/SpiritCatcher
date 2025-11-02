#include <stdlib.h>
#include <gbdk/emu_debug.h>

#include "Banks/SetAutoBank.h"
#include <gbdk/platform.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"

#include "GameTypes.h"
#include "SpriteOverPlayer.h"
#include "OverworldHud.h"

// saved last drawn values, to work out what to update on hud
static UINT8 last_hud_lives;
static UINT8 last_hud_stage;
static UINT8 last_hud_level;

extern Sprite* overplayer_sprite;

void Overworld_Hud_Init(void) BANKED {
    //EMU_printf("OverworldHud::Overworld_Hud_Init\n");
    IMPORT_MAP(overworldhud);
    INIT_HUD(overworldhud);
    last_hud_lives = 254;  // forced to 254, so that it they be updated on first call
    last_hud_stage = 254;
    last_hud_level = 254;
}

static UINT8 getHundreds(UINT8 full) {
    if (full < 100) return 0;
    UINT8 t = 0 ;
    while (full > 99) {
        full -= 100;
        t ++;
    }
    return t;
}

static UINT8 getTens (UINT8 full) {
    if (full < 10) return 0;
    UINT8 t = 0 ;
    while (full > 9) {
        full -= 10;
        t ++;
    }
    return t;
}

static void PutU16(UINT16 v, UINT8 at) {
    UINT8 hundreds;
    UINT8 tens;
    UINT8 ones;

    hundreds = getHundreds((UINT8) v);
    v -= hundreds*100;
    tens = getTens((UINT8) v);
    ones = v - tens*10;

    UPDATE_HUD_TILE(at++, 0, 1 + hundreds);
    UPDATE_HUD_TILE(at++, 0, 1 + tens);
    UPDATE_HUD_TILE(at++, 0, 1 + ones);
}

void Overworld_Hud_Update(void) BANKED {
    //EMU_printf("OverworldHud::Overworld_Hud_Update\n");
    OverPlayerData* player_data = (OverPlayerData*)overplayer_sprite->custom_data;
    //UINT8 tens;
    //UINT8 ones;

    if (last_hud_lives != g_player_lives) {
        for (UINT8 i = 0; i < MAX_LIVES; ++i) {
            UPDATE_HUD_TILE(18 - i, 1, i < g_player_lives ? 17 : 11);
        }
        last_hud_lives = g_player_lives;
    }
    if (last_hud_stage != g_stage_current) {
        UPDATE_HUD_TILE(5, 1, g_stage_current + 1); // +1 to skip the zero tile
        last_hud_stage = g_stage_current;
    }
    if (last_hud_level != g_level_current) {
        UPDATE_HUD_TILE(7, 1, g_level_current + 1); // +1 to skip the zero tile
        last_hud_level = g_level_current;
    }
}

