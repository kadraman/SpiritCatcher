#include "Banks/SetAutoBank.h"

#include <gbdk/platform.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "SpritePlayer.h"

#include "GlobalVars.h"

// saved last drawn values, to work out what to update on hud
static UINT8 last_spirits = 0;
static UINT8 last_coins = 0;
static UINT8 last_lives = 0;
static UINT16 last_timer = 0;

// level timer and countdown 
UINT16 level_max_time;        // maximum time for level
UINT16 timer_countdown;      // timer countdown to be decremented
static UINT8 timer_clock;    // frame counter for single timer tick
extern Sprite* player_sprite;

void Hud_Init(void) BANKED {
    IMPORT_MAP(hud);
    PlayerData* data = (PlayerData*)player_sprite->custom_data;
    INIT_HUD(hud);
    // prime the last values so they all get updated
    last_spirits = 0;
    last_coins = 0;
    last_lives = 0;
    timer_countdown = level_max_time;
    timer_clock = 0;
    last_timer = 0;
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

static void PutU16 (UINT16 v, UINT8 at)
{
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

void Hud_Update(void) BANKED {
    PlayerData* player_data = (PlayerData*)player_sprite->custom_data;
    UINT8 tens;
    UINT8 ones;

    timer_clock++;  
    if (timer_clock == 25) {
        timer_clock = 0;
        if (timer_countdown != 0) {
            //last_timer = 1;
            timer_countdown--;
        }
    }

#ifdef DEBUG_HUD
    // player position
    PutU16 (player_sprite->x, 6);
    PutU16 (player_sprite->y, 11);
    // current tile
    UINT8 tile = GetScrollTile((player_sprite->x + 8u) >> 3, (player_sprite->y + 16u) >> 3);
    PutU16(tile, 16);
    // animation frame
    UINT8 af = player_sprite->anim_frame;
    PutU16(af, 8);
#else
    if (last_timer != timer_countdown) {
        PutU16(timer_countdown, 13);
    }

    if (timer_countdown == 0) {
        PutU16(timer_countdown, 13);
        player_data->timeup = 1;
    }

    if (last_spirits != player_data->spirits) {
        last_spirits = player_data->spirits;
        tens = getTens(player_data->spirits);
        ones = player_data->spirits - (tens * 10);
        UPDATE_HUD_TILE(2, 0, 1 + tens);
        UPDATE_HUD_TILE(3, 0, last_spirits = 0 ? 1 : 1 + ones);
    }

    if (last_coins != player_data->coins) {
        last_coins = player_data->coins;
        tens = getTens(player_data->coins);
        ones = player_data->coins - (tens * 10);
        UPDATE_HUD_TILE(7, 0, 1 + tens);
        UPDATE_HUD_TILE(8, 0, last_coins = 0 ? 1 : 1 + ones);
    }

    if (last_lives != player_data->lives) {
        last_lives = player_data->lives;
        for (UINT8 i = 0; i < MAX_LIVES; ++i) {
            UPDATE_HUD_TILE(19 - i, 0, i < player_data->lives ? 18 : 19);
        }
    }
#endif
}

