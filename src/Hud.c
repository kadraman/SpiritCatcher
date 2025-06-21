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
static UINT8 last_spirits;
static UINT8 last_magix;
static UINT8 last_lives;
static UINT8 last_weapon;
static UINT16 last_timer;

// level timer and countdown 
UINT16 level_max_time;       // maximum time for level
UINT16 timer_countdown;      // timer countdown to be decremented
static UINT8 timer_clock;    // frame counter for single timer tick

extern Sprite* player_sprite;
extern INT16 accel_x, accel_y;
extern UINT16 x_inc, y_inc;

void Hud_Init(void) BANKED {
#ifdef DEBUG_HUD
    IMPORT_MAP(debughud);
    INIT_HUD(debughud);
#else
    IMPORT_MAP(hud);
    INIT_HUD(hud);
#endif
    last_spirits = 254;  // forced to 254, so that it will be updated on first call
    last_weapon = pWeaponKnife; // default weapon is knife
    timer_countdown = level_max_time;
    UPDATE_HUD_TILE(0, 0, 14); // knife
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
    // animation frame
    tens = getTens(player_sprite->anim_frame);
    ones = player_sprite->anim_frame - (tens * 10);
    UPDATE_HUD_TILE(1, 0, 1 + tens);
    UPDATE_HUD_TILE(2, 0, 1 + ones);
    // player state
    PlayerState state = GetPlayerState();
    tens = getTens(state);
    ones = state - (tens * 10);
    UPDATE_HUD_TILE(18, 0, 1 + tens);
    UPDATE_HUD_TILE(19, 0, 1 + ones);
#else
    if (last_timer != timer_countdown) {
        PutU16(timer_countdown, 9);
    }

    if (timer_countdown == 0) {
        PutU16(timer_countdown, 9);
        FLAG_SET(player_data->flags, pTimeUpFlag);
    }

    if (last_weapon != player_data->weapon) {
        if (player_data->weapon == pWeaponKnife) {
            UPDATE_HUD_TILE(0, 0, 14); // knife
        }
        if (player_data->weapon == pWeaponMagix) {
            if (last_magix == 0) {
                 UPDATE_HUD_TILE(0, 0, 12); // magix - empty
            } else {
                 UPDATE_HUD_TILE(0, 0, 13); // magix - full
            }
        }
        last_weapon = player_data->weapon;
    }

    if (last_spirits != player_data->spirits) {
        EMU_printf("Spirits was: %u:%u\n", last_spirits, player_data->spirits);
        tens = getTens(player_data->spirits);
        ones = player_data->spirits - (tens * 10);
        UPDATE_HUD_TILE(4, 0, 1 + tens);
        UPDATE_HUD_TILE(5, 0, last_spirits == 0 ? 1 : 1 + ones);
        if (player_data->spirits == 0) {
            UPDATE_HUD_TILE(3, 0, 16);
        } else {    
            UPDATE_HUD_TILE(3, 0, 17);
        }
        last_spirits = player_data->spirits;
        EMU_printf("Spirits now: %u:%u\n", last_spirits, player_data->spirits);
    }

    if (last_magix != player_data->magix) {
        if (player_data->weapon == pWeaponMagix) {
            if (last_magix == 0) {
                 UPDATE_HUD_TILE(0, 0, 12); // magix - empty
            } else {
                 UPDATE_HUD_TILE(0, 0, 13); // magix - full
            }
        }
        last_magix = player_data->magix;
    }

    if (last_lives != player_data->lives) {
        for (UINT8 i = 0; i < MAX_LIVES; ++i) {
            UPDATE_HUD_TILE(19 - i, 0, i < player_data->lives ? 18 : 19);
        }
        last_lives = player_data->lives;
    }
    
#endif
}

