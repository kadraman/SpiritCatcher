#include <stdbool.h>
#include <stdlib.h>
#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include "ZGBMain.h"

#include "Banks/SetAutoBank.h"
#include "main.h"
#include "Keys.h"
#include "Scroll.h"
#include "SpriteManager.h"

#include "GameTypes.h"
#include "SpriteOverPlayer.h"

const UINT8 anim_idle[] = {3, 0, 1, 2};
const UINT8 anim_walk_across[] = {3, 0, 1, 2};
const UINT8 anim_walk_up[] = {3, 0, 1, 2};
const UINT8 anim_walk_down[] = {3, 0, 1, 2};

Sprite* overplayer_sprite = 0;
Sprite* overplayer_eyes_sprite = NULL;

void START() {
    OverPlayerData* data = (OverPlayerData*)THIS->custom_data;
    memset(data, 0, sizeof(OverPlayerData));
    data->start_x = THIS->x;
    data->start_y = THIS->y;

    overplayer_sprite = THIS; // Store the pointer to the player sprite
    SetSpriteAnim(THIS, anim_walk_across, DEFAULT_ANIM_SPEED);

    EMU_printf("SpriteOverPlayer::START: Player lives = %d\n", g_player_lives);
    // Add eyes sprite at the same position
    //overplayer_eyes_sprite = SpriteManagerAdd(SpriteOverPlayerEyes, THIS->x, THIS->y);
}

void UPDATE() {
    if (KEY_PRESSED(J_RIGHT)) {
        SetSpriteAnim(THIS, anim_walk_across, DEFAULT_ANIM_SPEED);
        THIS->mirror = NO_MIRROR;
        TranslateSprite(THIS, 1 << delta_time, 0);
    } else if (KEY_PRESSED(J_LEFT)) {
        SetSpriteAnim(THIS, anim_walk_across, DEFAULT_ANIM_SPEED);
        THIS->mirror = V_MIRROR;
        TranslateSprite(THIS, -1 << delta_time, 0);
    } else if (KEY_PRESSED(J_UP)) {
        SetSpriteAnim(THIS, anim_walk_up, DEFAULT_ANIM_SPEED);
        TranslateSprite(THIS, 0, -1 << delta_time);
    } else if (KEY_PRESSED(J_DOWN)) {
        SetSpriteAnim(THIS, anim_walk_down, DEFAULT_ANIM_SPEED);
        TranslateSprite(THIS, 0, 1 << delta_time);
    } else {
        SetSpriteAnim(THIS, anim_idle, DEFAULT_ANIM_SPEED);
    }

    // Update eyes sprite to follow player
    /*
    if (overplayer_eyes_sprite) {
        overplayer_eyes_sprite->x = THIS->x;
        if (THIS->anim_frame == 0) {
            overplayer_eyes_sprite->y = THIS->y;
        } else if (THIS->anim_frame == 1) {
            overplayer_eyes_sprite->y = THIS->y + 1;
        } else {
            overplayer_eyes_sprite->y = THIS->y - 2;
        }
        overplayer_eyes_sprite->mirror = THIS->mirror;

         // Synchronize animation and frame
        SetSpriteAnimFrame(overplayer_eyes_sprite, THIS->anim_frame);  
    }*/
}

void DESTROY() {
    // Optionally handle sprite destruction
}