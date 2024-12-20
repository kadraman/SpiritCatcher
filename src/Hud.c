#include "Banks/SetAutoBank.h"

#include <gbdk/platform.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "SpritePlayer.h"

#include "GlobalVars.h"

// saved last drawn values, to work out what to update on hud
//static UINT8 lastBullets = 0;
static UINT8 lastCoins = 0;
static UINT8 lastLives = 0;
static UINT16 lastTimer = 0;

// level timer and countdown 
UINT16 levelMaxTime;        // maximum time for level
UINT16 timerCountdown;      // timer countdown to be decremented
static UINT8 timerClock;    // frame counter for single timer tick
UINT16 tileNum;
extern Sprite* player_sprite;

void Hud_Init(void) BANKED {
    IMPORT_MAP(hud);
    PlayerData* data = (PlayerData*)player_sprite->custom_data;
    INIT_HUD(hud);
    // prime the last values so they all get updated
    //lastBullets = 0;
    lastCoins = 0;
    lastLives = 0;
    timerCountdown = levelMaxTime;
    timerClock = 0;
    lastTimer = 0;
    tileNum = 0;
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
    PlayerData* data = (PlayerData*)player_sprite->custom_data;
    UINT8 tens;
    UINT8 ones;

    timerClock++;  
    if (timerClock == 25) {
        timerClock = 0;
        if (timerCountdown != 0) {
            //lastTimer = 1;
            timerCountdown--;
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
    if (lastTimer != timerCountdown) {
        PutU16(timerCountdown, 10);
    }

    if (timerCountdown == 0) {
        PutU16(timerCountdown, 10);
        data->timeup = 1;
    }

    /*if (lastBullets != data->bullets) {
        lastBullets = data->bullets;
        tens = getTens(data->bullets);
        ones = data->bullets - (tens * 10);
        //UPDATE_HUD_TILE(2, 0, 1 + tens);
        //UPDATE_HUD_TILE(3, 0, lastBullets = 0 ? 1 : 1 + ones);
    }*/

    if (lastCoins != data->coins) {
        lastCoins = data->coins;
        tens = getTens(data->coins);
        ones = data->coins - (tens * 10);
        UPDATE_HUD_TILE(3, 0, 1 + tens);
        UPDATE_HUD_TILE(4, 0, lastCoins = 0 ? 1 : 1 + ones);
    }

    if (lastLives != data->lives) {
        lastLives = data->lives;
        for (UINT8 i = 0; i < MAX_LIVES; ++i) {
            UPDATE_HUD_TILE(18 - i, 0, i < data->lives ? 18 : 19);
        }
    }
#endif
}

