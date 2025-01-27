#include <stdlib.h>

#include "Banks/SetAutoBank.h"
#include <gbdk/platform.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"

#include "SpritePlayer.h"
#include "GameTypes.h"

//#define DEBUG_HUD   1

// saved last drawn values, to work out what to update on hud
static UINT8 last_has_spirit = 0;
static UINT8 last_magix = 0;
static UINT8 last_lives = 0;
static UINT16 last_timer = 0;

// level timer and countdown 
UINT16 level_max_time;       // maximum time for level
UINT16 timer_countdown;      // timer countdown to be decremented
static UINT8 timer_clock;    // frame counter for single timer tick

extern Sprite* player_sprite;
extern INT16 accel_x, accel_y;
extern UINT16 x_inc, y_inc;
extern PlayerState curPlayerState;

void Hud_Init(void) BANKED {
#ifdef DEBUG_HUD
    IMPORT_MAP(debughud);
    INIT_HUD(debughud);
#else
    IMPORT_MAP(hud);
    INIT_HUD(hud);
#endif
    // prime the last values so they all get updated
    last_has_spirit = 0;
    last_magix = 0;
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

void Hud_Update(void) BANKED {
    PlayerData* player_data = (PlayerData*)player_sprite->custom_data;
    UINT8 tens;
    UINT8 ones;

    timer_clock++;  
    if (timer_clock == 25) {
        timer_clock = 0;
        if (timer_countdown != 0) {
            timer_countdown--;
        }
    }

#ifdef DEBUG_HUD
    // player position
    PutU16 (player_sprite->x, 0);
    PutU16 (player_sprite->y, 3);
    // player acceleration
    //PutU16 (abs(accel_x), 7);
    //PutU16 (abs(x_inc), 10);
    //PutU16 (abs(accel_y), 14);
    //PutU16 (abs(y_inc), 17);
    // current tile
    //UINT8 tile = GetScrollTile((player_sprite->x + 8u) >> 3, (player_sprite->y + 16u) >> 3);
    //PutU16(tile, 14);
    // player state
    PutU16(curPlayerState, 16);
    // animation frame
    tens = getTens(player_sprite->anim_frame);
    ones = player_sprite->anim_frame - (tens * 10);
    UPDATE_HUD_TILE(1, 0, 1 + tens);
    UPDATE_HUD_TILE(2, 0, 1 + ones);
#else
    if (last_timer != timer_countdown) {
        PutU16(timer_countdown, 9);
    }

    if (timer_countdown == 0) {
        PutU16(timer_countdown, 9);
        FLAG_SET(player_data->flags, pTimeUpFlag);
    }

    if (last_has_spirit != FLAG_CHECK(player_data->flags, pCaughtSpiritFlag)) {
        last_has_spirit = FLAG_CHECK(player_data->flags, pCaughtSpiritFlag);
        UPDATE_HUD_TILE(5, 0, 17);
    }

    if (last_magix != player_data->magix) {
        last_magix = player_data->magix;
        tens = getTens(player_data->magix);
        ones = player_data->magix - (tens * 10);
        UPDATE_HUD_TILE(1, 0, 1 + tens);
        UPDATE_HUD_TILE(2, 0, last_magix = 0 ? 1 : 1 + ones);
    }

    if (last_lives != player_data->lives) {
        last_lives = player_data->lives;
        for (UINT8 i = 0; i < MAX_LIVES; ++i) {
            UPDATE_HUD_TILE(19 - i, 0, i < player_data->lives ? 18 : 19);
        }
    }
#endif
}

