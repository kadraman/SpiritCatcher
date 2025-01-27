#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>

#include "SpriteManager.h"
#include "ZGBMain.h"

#define DEFAULT_ANIM_SPEED		10u

const UINT8 anim_attack1[] = {4, 0, 1, 2, 3};

void START() {
	SetSpriteAnim(THIS, anim_attack1, DEFAULT_ANIM_SPEED);
}

void UPDATE() {
	UINT8 i;
	Sprite* spr;
	Sprite* spr2;

	if ((THIS->anim_frame+1) == anim_attack1[0]) {
		SpriteManagerRemove(THIS_IDX);
	} else {
		for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
			spr = sprite_manager_sprites[sprite_manager_updatables[i + 1u]];
			if (spr->type == SpriteSlime || spr->type == SpriteBat || spr->type == SpriteMushroom) {
				if (CheckCollision(THIS, spr)) {
					spr2 = SpriteManagerAdd(SpriteParticle, spr->x, spr->y-5);
					SpriteManagerRemove(i);
				}
			}
		}
	}
}

void DESTROY() {
}