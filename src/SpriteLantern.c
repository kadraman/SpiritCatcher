#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>

#include "SpriteManager.h"
#include "ZGBMain.h"

#define DEFAULT_ANIM_SPEED		10u

const UINT8 anim_lantern[] = {2, 0, 1};

void RemoveLantern(Sprite *sprite) BANKED {
	SpriteManagerRemove(THIS_IDX);
}

void START() {
	SetSpriteAnim(THIS, anim_lantern, DEFAULT_ANIM_SPEED);
}

void UPDATE() {

}

void DESTROY() {
}