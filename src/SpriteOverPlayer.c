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

#define DEFAULT_ANIM_SPEED 8

const UINT8 anim_idle[] = {1, 0};
const UINT8 anim_walk_across[] = {2, 0, 1};
const UINT8 anim_walk_up[] = {2, 2, 3};
const UINT8 anim_walk_down[] = {2, 4, 5};

Sprite* overplayer_sprite = 0;

void START() {
    overplayer_sprite = THIS; // Store the pointer to the player sprite
    SetSpriteAnim(THIS, anim_walk_across, DEFAULT_ANIM_SPEED);
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

    // check for collision with level sprite
    /*for (UINT8 i = 0u; i != sprite_manager_updatables[0]; ++i) {
        Sprite* spr = sprite_manager_sprites[sprite_manager_updatables[i + 1u]];
        if (spr->type == SpriteLevelOpen || spr->type == SpriteLevelClosed) {
            if (CheckCollision(THIS, spr) && spr->type == SpriteLevelOpen) {
                EMU_printf("OverPlayer::UPDATE: Collision with level sprite at (%u, %u)\n", spr->x, spr->y);
                // Handle collision with level sprite
                // For example, you might want to change the state or trigger an event
                // SetState(StateOverworld); // Example action
            }   
        }
    }*/
}

void DESTROY() {
    // Optionally handle sprite destruction
}