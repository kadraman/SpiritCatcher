#include <stdlib.h>
#include <gbdk/emu_debug.h>

#include "Banks/SetAutoBank.h"
#include <gbdk/platform.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"

#include "GameTypes.h"
#include "SpritePlayer.h"
#include "Hud.h"

// saved last drawn values, to work out what to update on hud
static UINT8 last_hud_lives;
extern Sprite* overplayer_sprite;

void Overworld_Hud_Init(void) BANKED {
    IMPORT_MAP(overworldhud);
    INIT_HUD(overworldhud);
    last_hud_lives = 0xFF; // force update of lives
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
    UINT8 tens;
    UINT8 ones;

    /*if (last_lives != player_data->lives) {
        for (UINT8 i = 0; i < MAX_LIVES; ++i) {
            UPDATE_HUD_TILE(19 - i, 0, i < player_data->lives ? 18 : 19);
        }
        last_lives = player_data->lives;
    }*/
    
}

