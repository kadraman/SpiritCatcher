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

const UINT8 anim_eyes[] = {3, 0, 0, 0};

extern Sprite* overplayer_eyes_sprite;

void START() {
    overplayer_eyes_sprite = THIS; // Store the pointer to the player sprite
    
    //SetSpriteAnim(THIS, anim_eyes, DEFAULT_ANIM_SPEED);
}

void UPDATE() {
   // Optional handle sprite updates
}

void DESTROY() {
    // Optionally handle sprite destruction
}