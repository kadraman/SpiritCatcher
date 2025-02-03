#include "Banks/SetAutoBank.h"
#include "main.h"

#include "SpriteManager.h"

#include "ZGBMain.h"
#include "Scroll.h"

#include "Sound.h"

const UINT8 anim_magix[] = {4, 0, 1, 2, 3};


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
		if (spr->type == SpriteSlime || spr->type == SpriteBat || spr->type == SpriteMushroom) {
			if(CheckCollision(THIS, spr)) {
				//if(spr->type != XXXX || spr->anim_data[1 + spr->anim_frame] == 1u) { //XXX can only die on frame 1
				//	SpriteManagerRemove(i);
				//	PlayFx(CHANNEL_4, 20, 0x0d, 0xff, 0x7d, 0xc0);
				//}
				spr2 = SpriteManagerAdd(SpriteParticle, spr->x, spr->y);
				SpriteManagerRemove(i);
				break;
			}
		}
	}
}

void DESTROY() {
}