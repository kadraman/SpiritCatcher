#include "Banks/SetAutoBank.h"
#include "main.h"

#include "SpriteManager.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "StateGame.h"

#include "Sound.h"

const UINT8 anim_magix[] = {1, 0};


void START() {
	PlayFx(CHANNEL_1, 10, 0x2C, 0xD8, 0xF3, 0x62, 0xC7);
	SetSpriteAnim(THIS, anim_magix, 20u);
}

void UPDATE() {
	UINT8 i;
	Sprite* spr;
	Sprite* spr2;
	
	if(THIS->mirror ==  V_MIRROR) {
		THIS->x -= 3 << delta_time;
	} else {
		THIS->x += 3 << delta_time;
	}

	if(scroll_collisions[GetScrollTile((THIS->x + 4)>> 3, (THIS->y + 4) >> 3)]) {
		SpriteManagerRemove(THIS_IDX);
		return;
	}

	for(i = 0u; i != sprite_manager_updatables[0]; ++i) {
		spr = sprite_manager_sprites[sprite_manager_updatables[i + 1u]];
		if (spr->type == SpriteSlime || spr->type == SpriteBat || spr->type == SpriteRockard) {
			if (CheckCollision(THIS, spr)) {
				spr2 = SpriteManagerAdd(SpriteParticle, spr->x, spr->y);
				ClearMapEntity(spr);
				SpriteManagerRemove(i);
				SpriteManagerRemove(THIS_IDX);
				break;
			}
		}
	}
}

void DESTROY() {
}