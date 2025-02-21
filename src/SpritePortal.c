#include <stdbool.h>
#include <gbdk/emu_debug.h>
#include "main.h"

#include "Banks/SetAutoBank.h"
#include "Vector.h"
#include "SpriteManager.h"
#include "SpritePlayer.h"

#include "GameTypes.h"

#define DEFAULT_ANIM_SPEED		10u
#define DISAPPEAR_ANIM_SPEED 	10u

const UINT8 anim_portal_appear[] = VECTOR(10, 6, 6, 5, 4, 3, 2, 1, 5, 6, 6);
const UINT8 anim_portal_stable[] = VECTOR(5, 0, 1, 2, 3, 4);
const UINT8 anim_portal_disappear[] = VECTOR(3, 5, 6, 6);
extern UINT8 item_collected;
UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* collectable, ItemType itype) BANKED;
struct PortalInfo
{
    UINT8 frame;
	UINT8 appear;
	UINT8 stable;
	UINT8 disappear;
};
extern Sprite* player_sprite;
extern UINT8 level_complete;
 
void START() {
	struct PortalInfo* data = (struct PortalInfo*)THIS->custom_data;
	PlayerData* player_data = (PlayerData*)player_sprite->custom_data;
	data->appear = false;
	data->stable = false;
	data->disappear = false;
	THIS->visible = false;
	SetSpriteAnim(THIS, anim_portal_stable, DEFAULT_ANIM_SPEED);
}

void UPDATE() {
	struct PortalInfo* data = (struct PortalInfo*)THIS->custom_data;
	PlayerData* player_data = (PlayerData*)player_sprite->custom_data;
	if (FLAG_CHECK(player_data->flags, pCaughtSpiritFlag)) {
		THIS->visible = true;
		data->stable = true;
		EMU_printf("SpritePortal::%s portal visible\n", __func__);
		//SetSpriteAnim(THIS, anim_portal_stable, DEFAULT_ANIM_SPEED);
	} else {
		EMU_printf("SpritePortal::%s portal not visible\n", __func__);
	}
	// if exit portal and not set to disappear change animation state
	if (level_complete && THIS->visible) {
		EMU_printf("SpritePortal::%s level complete - portal disappearing\n", __func__);
		data->stable = false;
		data->disappear = true;
		SetSpriteAnim(THIS, anim_portal_disappear, DISAPPEAR_ANIM_SPEED);
	}
	// if exit portal remove it after X frames
	if (level_complete && data->disappear && THIS->anim_frame == VECTOR_LEN(anim_portal_disappear)-1) {
		EMU_printf("SpritePortal::%s level complete - portal remove\n", __func__);
		TakeCollectable(THIS, ITEM_PORTAL);
		SpriteManagerRemove(THIS_IDX);
	}
	//data->frame++;
}

void DESTROY() {
}