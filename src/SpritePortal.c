#include <stdbool.h>
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
	data->appear = data->stable = data->disappear = false;
	if (FLAG_CHECK(player_data->flags, pCaughtSpiritFlag)) {
		// TODO: define a better way to define if its the starting portal
		if (THIS->x < 32 && (player_sprite->x < 32 && player_sprite->y < 144)) {
			data->appear = true;
			SetSpriteAnim(THIS, anim_portal_appear, DISAPPEAR_ANIM_SPEED);
		} else {
			data->stable = true;
			SetSpriteAnim(THIS, anim_portal_stable, DEFAULT_ANIM_SPEED);
		}
	} else {
		SpriteManagerRemove(THIS_IDX);
	}
}

void UPDATE() {
	struct PortalInfo* data = (struct PortalInfo*)THIS->custom_data;
	// if entry portal remove it after X frames
	if (data->appear && (THIS->anim_frame+1) == VECTOR_LEN(anim_portal_appear)) {
		TakeCollectable(THIS, ITEM_PORTAL);
		SpriteManagerRemove(THIS_IDX);
	}
	// if exit portal and not set to disappear change animation state
	if (level_complete && data->stable) {
		data->stable = false;
		data->disappear = true;
		SetSpriteAnim(THIS, anim_portal_disappear, DISAPPEAR_ANIM_SPEED);
		return;
	}
	// if exit portal remove it after X frames
	if (level_complete && data->disappear && THIS->anim_frame == VECTOR_LEN(anim_portal_disappear)-1) {
		TakeCollectable(THIS, ITEM_PORTAL);
		SpriteManagerRemove(THIS_IDX);
	}
	//data->frame++;
}

void DESTROY() {
}