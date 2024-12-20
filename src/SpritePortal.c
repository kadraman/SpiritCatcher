#include <stdbool.h>
#include "Banks/SetAutoBank.h"
#include "main.h"

#include "SpriteManager.h"


const UINT8 anim_portal[] = {5, 0, 1, 2, 3, 4};
const UINT8 anim_potral_dissapear[] = {8, 0, 1, 2, 3, 4, 5, 6, 7};
const UINT8 anim_portal_appear[] = {8, 7, 6, 5, 0, 1, 2, 3, 4};
struct PortalInfo
{
    UINT8 frame;
};
extern Sprite* player_sprite;
extern UINT8 player_spawned;
 
void START() {
	struct PortalInfo* data = (struct PortalInfo*)THIS->custom_data;
	if (player_spawned) {
		SetSpriteAnim(THIS, anim_portal_appear, 5u);
	} else {
		SetSpriteAnim(THIS, anim_portal, 5u);
	}
	THIS->anim_speed = 33u;
}

void UPDATE() {
	struct PortalInfo* data = (struct PortalInfo*)THIS->custom_data;
	data->frame++;
	if (player_spawned && THIS->anim_frame == 7) {
		player_spawned = false;
		SpriteManagerRemove(THIS_IDX);
	}
}

void DESTROY() {
}