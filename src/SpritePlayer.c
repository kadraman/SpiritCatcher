#include <stdbool.h>
#include <stdlib.h>
#include <gbdk/emu_debug.h>

#include "Banks/SetAutoBank.h"
#include "gb/gb.h"
#include "Keys.h"
#include "Scroll.h"
#include "Print.h"
#include "Sound.h"
#include "Vector.h"

#include "GameTypes.h"
#include "Hud.h"
#include "StateGame.h"
#include "SpritePlayer.h"

// player animations - the first number indicates the number of frames
const UINT8 anim_walk_idle[] = {1, 0};		
const UINT8 anim_walk_idle_attack[] = {1, 16};		
const UINT8 anim_walk[] = {4, 1, 3, 4, 5};
const UINT8 anim_walk_attack[] = {1, 16};
const UINT8 anim_jump[] = {2, 6, 7};
const UINT8 anim_fall[] = {1, 8};
const UINT8 anim_jump_attack[] = {1, 16};
const UINT8 anim_attack[] = {2, 16, 17};
const UINT8 anim_catch[] = {1, 18};
const UINT8 anim_climb[] = {3, 19, 20, 21};
const UINT8 anim_climb_idle[] = {1, 20};
const UINT8 anim_hit[] = {4, 9, 10, 9, 10};
const UINT8 anim_die[] = {16, 9, 10, 9, 10, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12};
//const UINT8 anim_appear[] = {3, 15, 14, 13};
//const UINT8 anim_disappear[] = {5, 14, 14, 13, 14, 15};
const UINT8 anim_drown[] = {10, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15};
//const UINT8 anim_victory[] = {2, 21, 22}; // TBD

Sprite* player_sprite = 0;
Sprite* attack1_sprite = 0;
Sprite* lantern_sprite = 0;
extern Sprite* attack_particle;
extern UINT8 start_x, start_y;
extern INT16 scroll_x, scroll_y;
extern INT16 min_x, max_x, min_y, max_y;
PlayerState curPlayerState, prevPlayerState;

INT16 accel_y;
INT16 accel_x;
UINT16 x_inc;
UINT16 y_inc;
UINT8 reset_x;
UINT8 reset_y;
UINT8 magix_cooldown;
UINT8 magix_recharge;
UINT8 invincible_secs;
UINT8 invincible_ticks;
UINT8 anim_hit_counter;
UINT8 anim_hit_ticks;
UINT8 visible_skip;
UINT8 tile_collision;
UINT8 prev_keys;

UINT8 adjacent_tile;

extern UINT8 g_level_complete;

extern UINT16 timer_countdown;
extern UINT16 level_max_time;
extern UINT16 level_width;
extern UINT16 level_height;

void UpdateAttackPos(void);
void Jump(void);
void Hit(void);
void Drown(void);
void Slash(void);
void Magix(void);
void CatchSpirit(void);
void ChangeWeapon(void);
void CheckOnPlatform(void);
void CheckCollisionTile(Sprite* sprite, UINT8 idx);
void ApplyGravity(Sprite* sprite, UINT8 idx);
void AddDampening(Sprite* sprite, UINT8 idx);
void ApplyMovementX(Sprite* sprite, UINT8 idx);
void CheckCanClimb(void);
void CheckLevelComplete(void);
void UpdateSpawn(void);
void UpdateIdle(void);
void UpdateWalking(void);
void UpdateJumping(void);
void UpdateFalling(void);
void UpdateAttacking(void);
void UpdateClimbing(void);
void UpdatePlatform(void);
void UpdateHit(void);
void UpdateDie(void);
void UpdateDrown(void);
void UpdateTimeUp(void);
void UpdateVictory(void);

void RemoveLantern(Sprite *sprite) BANKED;

// attack function
typedef void (*attack_func_t)(void);
attack_func_t attack_function;

void SetPlayerState(PlayerState state) BANKED {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	prevPlayerState = curPlayerState;
	curPlayerState = state;
	switch (state) {
		case PLAYER_STATE_SPAWN:
			SetSpriteAnim(THIS, anim_walk_idle, DEFAULT_ANIM_SPEED); break;
		case PLAYER_STATE_IDLE:
			SetSpriteAnim(THIS, anim_walk_idle, DEFAULT_ANIM_SPEED); break;
		case PLAYER_STATE_WALK:
			SetSpriteAnim(THIS, anim_walk, WALK_ANIM_SPEED); break;
		case PLAYER_STATE_JUMP:
			SetSpriteAnim(THIS, anim_jump, DEFAULT_ANIM_SPEED); break;
		case PLAYER_STATE_FALL:
			SetSpriteAnim(THIS, anim_fall, DEFAULT_ANIM_SPEED); break;
		case PLAYER_STATE_ATTACK:
			SetSpriteAnim(THIS, anim_attack, DEFAULT_ANIM_SPEED); break;
		case PLAYER_STATE_CATCH:
			SetSpriteAnim(THIS, anim_catch, DEFAULT_ANIM_SPEED); break;
		case PLAYER_STATE_CLIMB:
			SetSpriteAnim(THIS, anim_climb, DEFAULT_ANIM_SPEED); break;
		case PLAYER_STATE_HIT:
			SetSpriteAnim(THIS, anim_hit, HIT_ANIM_SPEED); break;
		case PLAYER_STATE_DIE:
			SetSpriteAnim(THIS, anim_die, DIE_ANIM_SPEED); break;
		case PLAYER_STATE_DROWN:
			SetSpriteAnim(THIS, anim_drown, DROWN_ANIM_SPEED); break;
		case PLAYER_STATE_TIMEUP:
			SetSpriteAnim(THIS, anim_die, DIE_ANIM_SPEED); break;
		case PLAYER_STATE_APPEAR:
			SetSpriteAnim(THIS, anim_walk_idle, DEFAULT_ANIM_SPEED); break;
		case PLAYER_STATE_DISAPPEAR:
		case PLAYER_STATE_VICTORY:
			SetSpriteAnim(THIS, anim_walk_idle, DEFAULT_ANIM_SPEED); break;
		default:
			break;
	}
}

PlayerState GetPlayerState(void) BANKED {
	return curPlayerState;
}

PlayerState GetPreviousPlayerState(void) BANKED {
	return prevPlayerState;
}

void SetPreviousPlayerState(void) BANKED {
	if (prevPlayerState == curPlayerState) {
		SetPlayerState(PLAYER_STATE_IDLE);
	} else {
		return SetPlayerState(prevPlayerState);
	}
}

bool GetPlayerStateEquals(PlayerState ps) BANKED {
	return curPlayerState == ps;
}

UINT8 GetLastAnimFrame(void) BANKED {
	return VECTOR_LEN(THIS->anim_data)-1;
}


bool PlayerIsOnTopOfPlatform(Sprite* player, Sprite* platform) BANKED {
    // Player's feet position (center of the bottom of the player sprite)
    UINT16 player_feet_x = player->x + (player->coll_w >> 1);
    UINT16 player_feet_y = player->y + player->coll_h; // bottom of player

    // Platform bounds
    UINT16 plat_left = platform->x;
    UINT16 plat_right = platform->x + platform->coll_w;
    UINT16 plat_top = platform->y;

    // Only on platform if feet are horizontally within platform,
    // and vertically just above the top (allowing a small tolerance)
    return (
        player_feet_x >= plat_left - 1 &&
        player_feet_x <= plat_right + 1 &&
        player_feet_y >= plat_top - 2 &&
        player_feet_y <= plat_top + 2
    );
}

void UpdateAttackPos(void) {
	attack1_sprite->mirror = THIS->mirror;
	if (THIS->mirror == V_MIRROR) 
		attack1_sprite->x = THIS->x - 14u;
	else
		attack1_sprite->x = THIS->x + 14u; 
	attack1_sprite->y = THIS->y + 2u;
}

void Jump(void) {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	FLAG_CLEAR(data->flags, pGroundedFlag);
	FLAG_CLEAR(data->flags, pOnPlatformFlag);
	//PlayFx(CHANNEL_1, 5, 0x17, 0x9f, 0xf3, 0xc9, 0xc4);
	PlayFx(CHANNEL_1, 5, 0x19, 0x81, 0x65, 0x8c, 0x85);

	SetPlayerState(PLAYER_STATE_JUMP);
	accel_y = -Y_JUMP_HEIGHT;
}

void Hit(void) {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	if (GetPlayerState() == PLAYER_STATE_HIT) return;
	if (FLAG_CHECK(data->flags, pInvincibleFlag)) return;
	if (data->lives == 1) { // last life
		SetPlayerState(PLAYER_STATE_DIE);
		PlayFx(CHANNEL_1, 10, 0x5b, 0x7f, 0xf7, 0x15, 0x86);
		//invincible_secs = 10;
	} else {
		// knockback
		if (THIS->mirror == NO_MIRROR) {
			THIS->x = THIS->x - 4;
		} else { // THIS->mirror == V_MIRROR
			THIS->x = THIS->x + 4;
		}
		//SetPlayerState(PLAYER_STATE_HIT);
		PlayFx(CHANNEL_1, 10, 0x5b, 0x7f, 0xf7, 0x15, 0x86);
		FLAG_SET(data->flags, pInvincibleFlag);
		if (data->lives > 0 && data->lives < UCHAR_MAX) data->lives--;
		invincible_secs = 10;
		anim_hit_counter = 10;
	}
	Hud_Update();	
}

void Drown(void) {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	if (GetPlayerState() == PLAYER_STATE_DROWN) return;
	SetPlayerState(PLAYER_STATE_DROWN);
	PlayFx(CHANNEL_1, 10, 0x5b, 0x7f, 0xf7, 0x15, 0x86);
	//THIS->y = THIS->y + 1;
	//invincible_secs = 10;
}

void Slash(void) {
	SetPlayerState(PLAYER_STATE_ATTACK);
	attack1_sprite = SpriteManagerAdd(SpriteAttack1, THIS->x, THIS->y);
	UpdateAttackPos();
	//PlayFx(CHANNEL_4, 20, 0x0d, 0xff, 0x7d, 0xc0);
	PlayFx(CHANNEL_4, 10, 0x14, 0x51, 0x21, 0xc0);

}

void Magix(void) {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	// no magix left
	if (data->magix == 0) return;
	SetPlayerState(PLAYER_STATE_ATTACK);
	Sprite* magix_sprite = SpriteManagerAdd(SpriteMagix, 0, 0);
	magix_sprite->mirror = THIS->mirror;
	if (THIS->mirror) {
		magix_sprite->x = THIS->x - 2u;
	} else {
		magix_sprite->x = THIS->x + 7u; 
	}	
	magix_sprite->y = THIS->y + 5u;
	data->magix--;
	// reset cooldown/coolup
	//magix_cooldown = MAGIX_COOLDOWN_TIME;
	//magix_recharge = MAGIX_RECHARGE_TIME;
	SetPlayerState(prevPlayerState);
}

void CatchSpirit(void) {
	//EMU_printf("SpritePlayer::%s catching spirit\n", __func__);
	SetPlayerState(PLAYER_STATE_CATCH);
	if (THIS->mirror == V_MIRROR) {
        // Facing left
        lantern_sprite = SpriteManagerAdd(SpriteLantern, THIS->x - 6, THIS->y - 7);
    } else {
        // Facing right
        lantern_sprite = SpriteManagerAdd(SpriteLantern, THIS->x + 10, THIS->y - 7);
    }
	PlayFx(CHANNEL_4, 20, 0x0d, 0xff, 0x7d, 0xc0);
}

void ChangeWeapon(void) {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	EMU_printf("SpritePlayer::%s current weapon:%u\n", __func__, data->weapon);
	if (data->weapon >= MAX_WEAPON) {
		data->weapon = 0; // reset to first weapon
	} else {
		data->weapon++;
	}
	switch (data->weapon) {
		case pWeaponKnife:
			attack_function = Slash;
			break;
		case pWeaponMagix:
			attack_function = Magix;
			break;
		default:
			EMU_printf("SpritePlayer::%s unknown weapon:%u\n", __func__, data->weapon);
			break;
	}
	EMU_printf("SpritePlayer::%s changed weapon:%u\n", __func__, data->weapon);
	Hud_Update();
}

void CheckOnPlatform(void) {
	// TODO: optimise this
	PlayerData* data = (PlayerData*)THIS->custom_data;
	if (FLAG_CHECK(data->flags, pOnPlatformFlag)) {

		for (UINT8 i = 0u; i != sprite_manager_updatables[0]; ++i) {
			Sprite* spr = sprite_manager_sprites[sprite_manager_updatables[i + 1u]];
			if (spr->type == SpritePlatform) {
				if (CheckCollision(THIS, spr)) {
					//EMU_printf("player on platform\n", tile_collision);
					FLAG_SET(data->flags, pOnPlatformFlag);
					SetPlayerState(PLAYER_STATE_PLATFORM);
				} else {
					//EMU_printf("player off platform\n", tile_collision);
					FLAG_CLEAR(data->flags, pOnPlatformFlag);
					SetPlayerState(PLAYER_STATE_IDLE);
				}
			}
		}

	}
}

void CheckCollisionTile(Sprite* sprite, UINT8 idx) {
	//EMU_printf("tile collision: %u\n", tile_collision);
	switch (tile_collision) {
		case TILE_INDEX_SPIKE_UP:
		case TILE_INDEX_SPIKE_DOWN:
			//Hit(sprite, idx);
			break;
		case TILE_INDEX_WATER_1:
		case TILE_INDEX_WATER_2:
		case TILE_INDEX_WATER_3:
			Drown();
			break;
		default:	
			// ignore
			break;
	}
}

void ApplyGravity(Sprite* sprite, UINT8 idx) {
	PlayerData* data = (PlayerData*)sprite->custom_data;
	//if (FLAG_CHECK(data->flags, pGroundedFlag)) return;
	// simple gravity physics - unless climbinig or on platform
	if (accel_y < Y_SPEED_MAX && !FLAG_CHECK(data->flags, pOnPlatformFlag)) {
		accel_y += Y_GRAVITY;
	}
	tile_collision = TranslateSprite(sprite, 0, accel_y >> 6 << delta_time);
	if (!tile_collision && delta_time != 0 && accel_y < Y_SPEED_MAX) { 
		//do another iteration if there is no collision
		accel_y += Y_GRAVITY;
		tile_collision = TranslateSprite(sprite, 0, accel_y >> 6 << delta_time);
	}
	if (tile_collision) {
		//EMU_printf("SpritePlayer::%s tile collision: ud\n", __func__, tile_collision);
		accel_y = 0;
		FLAG_SET(data->flags, pGroundedFlag);
		CheckCollisionTile(sprite, idx);
	}
}

void AddDampening(Sprite* sprite, UINT8 idx) {
	if (accel_x > 0) {
		accel_x -= X_DAMPENING;
		x_inc = accel_x >> 6;
		tile_collision = TranslateSprite(sprite, x_inc << delta_time, 0);
	} else if (accel_x < 0) {
		accel_x += X_DAMPENING;
		x_inc = abs(accel_x) >> 6;
		tile_collision = TranslateSprite(sprite, -x_inc << delta_time, 0);
	}
	CheckCollisionTile(sprite, idx);
}

void ApplyMovementX(Sprite* sprite, UINT8 idx) {
	PlayerData* data = (PlayerData*)sprite->custom_data;
	if (KEY_PRESSED(J_RIGHT)) {
		if (accel_x < (X_SPEED_MAX-X_SPEED_INC)) accel_x += X_SPEED_INC;	
		x_inc = accel_x >> 6;	
		tile_collision = TranslateSprite(sprite, x_inc << delta_time, 0);
		CheckCollisionTile(sprite, idx);
		sprite->mirror = NO_MIRROR;
		//CheckOnPlatform();
	} else if (KEY_PRESSED(J_LEFT)) {
		if (accel_x > -(X_SPEED_MAX-X_SPEED_INC)) accel_x -= X_SPEED_INC;
		x_inc = abs(accel_x) >> 6;
		if (sprite->x - x_inc < min_x) {
			x_inc = accel_x = 0;
		}
		tile_collision = TranslateSprite(sprite, -x_inc << delta_time, 0);
		CheckCollisionTile(sprite, idx);
		sprite->mirror = V_MIRROR;
		//CheckOnPlatform();
	} else {
		if (FLAG_CHECK(data->flags, pGroundedFlag)) {
			AddDampening(sprite, idx);
		}
	}
}

void CheckCanClimb(void) {
	if (KEY_PRESSED(J_UP)) {
		adjacent_tile = GetScrollTile((THIS->x + 8u) >> 3, (THIS->y + 4u) >> 3);
		if (adjacent_tile == TILE_INDEX_LADDER_LEFT || adjacent_tile == TILE_INDEX_LADDER_RIGHT) {
			THIS->y = THIS->y - 1u;
			accel_y = 0;
			SetPlayerState(PLAYER_STATE_CLIMB);
		}
	} else if (KEY_PRESSED(J_DOWN)) {
		adjacent_tile = GetScrollTile((THIS->x + 8u) >> 3, (THIS->y + 16u) >> 3);
		if (adjacent_tile == TILE_INDEX_LADDER_LEFT || adjacent_tile == TILE_INDEX_LADDER_RIGHT) {
			THIS->y = THIS->y + 1u;
			accel_y = 0;
			SetPlayerState(PLAYER_STATE_CLIMB);
		}
	}
}

void CheckLevelComplete(void) {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	// if the spirit has not been caught we can ignore
	if (!FLAG_CHECK(data->flags, pCaughtSpiritFlag)) return;
	if (THIS->mirror == NO_MIRROR) {
		// moving right
		adjacent_tile = GetScrollTile((THIS->x + 8u) >> 3, (THIS->y) >> 3);
		if (adjacent_tile >= TILE_INDEX_PORTAL_TOPLEFT && adjacent_tile <= TILE_INDEX_PORTAL_BOTTOMRIGHT) {
			THIS->x = THIS->x + 8u;
			accel_y = 0;
			SetPlayerState(PLAYER_STATE_VICTORY);
		}
	} else { // THIS->mirror == V_MIRROR
		// moving left
		adjacent_tile = GetScrollTile((THIS->x - 8u) >> 3, (THIS->y) >> 3);
		if (adjacent_tile >= TILE_INDEX_PORTAL_TOPLEFT && adjacent_tile <= TILE_INDEX_PORTAL_BOTTOMRIGHT) {
			THIS->x = THIS->x - 8u;
			accel_y = 0;
			SetPlayerState(PLAYER_STATE_VICTORY);
		}
	}
}

//

void START() {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	memset(data, 0, sizeof(PlayerData));
	g_player_dead = false;
	player_sprite = THIS;
	data->lives = MAX_LIVES;
	data->start_x = THIS->x;
	data->start_y = THIS->y;
	FLAG_SET(data->flags, pGroundedFlag);
	data->magix = 0;
	data->weapon = pWeaponKnife;
	attack_function = Slash;
	accel_y = 0;
	accel_x = 0;
	//magix_cooldown = 0;
	g_level_complete = false;
	scroll_target = THIS;
	reset_x = 20;
	reset_y = 80;
	attack1_sprite = 0;
	anim_hit_counter = 0;
	visible_skip = 0;
	tile_collision = 0;
	invincible_secs = 0;
	prev_keys = 0;
	SetPlayerState(PLAYER_STATE_SPAWN);
}

void UpdateSpawn(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
	//if (THIS->anim_frame == GetLastAnimFrame()) {
		SetPlayerState(PLAYER_STATE_IDLE);
	//}
}	
void UpdateIdle(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
	PlayerData* data = (PlayerData*)THIS->custom_data;
	ApplyGravity(THIS, THIS_IDX);
	if (KEY_PRESSED(J_RIGHT) || KEY_PRESSED(J_LEFT)) {
		SetPlayerState(PLAYER_STATE_WALK);
	}
	CheckCanClimb();
	CheckLevelComplete();
	if (KEY_TICKED(J_A) && (FLAG_CHECK(data->flags, pGroundedFlag) || FLAG_CHECK(data->flags, pOnPlatformFlag))) {
		Jump();
	}
	if ((keys & J_UP) && (keys & J_B)) CatchSpirit();
	else if (KEY_TICKED(J_B)) attack_function();
	if (KEY_TICKED(J_SELECT)) ChangeWeapon();
}
UINT8 idle_reset = 0;
void UpdateWalking(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
	PlayerData* data = (PlayerData*)THIS->custom_data;
	ApplyMovementX(THIS, THIS_IDX);
	ApplyGravity(THIS, THIS_IDX);
	CheckCanClimb();
	CheckLevelComplete();
	if (KEY_TICKED(J_A) && (FLAG_CHECK(data->flags, pGroundedFlag))) {
		Jump();
	}
	if ((keys & J_UP) && (keys & J_B)) CatchSpirit();
	else if (KEY_TICKED(J_B)) attack_function();
	if (KEY_TICKED(J_SELECT)) ChangeWeapon();
	if (keys == 0) {
		//SetFrame(THIS, 1);
		idle_reset++; 
	}
	if (idle_reset == 30) {
		SetPlayerState(PLAYER_STATE_IDLE);
		idle_reset = 0;
	}
}
void UpdateJumping(void) {	
	//EMU_printf("SpritePlayer::%s\n", __func__);
	PlayerData* data = (PlayerData*)THIS->custom_data;
	ApplyMovementX(THIS, THIS_IDX);
	ApplyGravity(THIS, THIS_IDX);
	CheckLevelComplete();
	// check if now FALLING?
	if ((accel_y >> 6) > 1) {
		SetPlayerState(PLAYER_STATE_FALL);
	}
	if ((FLAG_CHECK(data->flags, pGroundedFlag))) {
		SetPlayerState(PLAYER_STATE_IDLE);
	}
	if (KEY_TICKED(J_B)) attack_function();
	if (KEY_TICKED(J_SELECT)) ChangeWeapon();
}
void UpdateFalling(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
	PlayerData* data = (PlayerData*)THIS->custom_data;
	ApplyMovementX(THIS, THIS_IDX);
	ApplyGravity(THIS, THIS_IDX);
	CheckLevelComplete();
	if ((FLAG_CHECK(data->flags, pGroundedFlag))) {
		SetPlayerState(PLAYER_STATE_IDLE);
	}
	if (KEY_TICKED(J_B)) attack_function();
	if (KEY_TICKED(J_SELECT)) ChangeWeapon();
}
void UpdateAttacking(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
	PlayerData* data = (PlayerData*)THIS->custom_data;
	if (prevPlayerState != PLAYER_STATE_CLIMB) {
		ApplyMovementX(THIS, THIS_IDX);
		ApplyGravity(THIS, THIS_IDX);
	}
	UpdateAttackPos();
	if (THIS->anim_frame == GetLastAnimFrame()) {
		SetPlayerState(prevPlayerState);
	}
}
void UpdateCatching(void) {
	//EMU_printf("SpritePlayer::%s; previous state:%d\n", __func__, GetPreviousPlayerState());
	if ( ( (prev_keys & J_UP) && !(keys & J_UP) ) ||
		( (prev_keys & J_B)  && !(keys & J_B) ) ) {
		// Either J_UP or J_B was just released
		//EMU_printf("SpritePlayer::%s stopped catching\n", __func__);
		SpriteManagerRemoveSprite(lantern_sprite);
		SetPlayerState(PLAYER_STATE_IDLE);
	}
}
void UpdateClimbing(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
	UINT8 i = TILE_INDEX_LADDER_LEFT;
	if (KEY_PRESSED(J_UP)) {
		SetSpriteAnim(THIS, anim_climb, DEFAULT_ANIM_SPEED);
		tile_collision = TranslateSprite(THIS, 0, -1 << delta_time);
		CheckCollisionTile(THIS, THIS_IDX);
		if (tile_collision == TILE_INDEX_LADDER_LEFT || tile_collision == TILE_INDEX_LADDER_RIGHT) {
			THIS->y = THIS->y - 1;
		}
		i = GetScrollTile((THIS->x + 8u) >> 3, (THIS->y + 16u) >> 3);
	} else if (KEY_PRESSED(J_DOWN)) {
		SetSpriteAnim(THIS, anim_climb, DEFAULT_ANIM_SPEED);
		tile_collision = TranslateSprite(THIS, 0, 1 << delta_time);
		CheckCollisionTile(THIS, THIS_IDX);
		if (tile_collision == TILE_INDEX_LADDER_LEFT || tile_collision == TILE_INDEX_LADDER_RIGHT) {
			THIS->y = THIS->y + 1;
		}
		i = GetScrollTile((THIS->x + 8u) >> 3, (THIS->y + 16u) >> 3);
	} else {
		SetSpriteAnim(THIS, anim_climb_idle, DEFAULT_ANIM_SPEED);
	}
	if (KEY_PRESSED(J_RIGHT)) {
		THIS->mirror = NO_MIRROR;
	} else if(KEY_PRESSED(J_LEFT)) {
		THIS->mirror = V_MIRROR;
	}

	//Check the end of the ladder
	if (i != TILE_INDEX_LADDER_LEFT && i != TILE_INDEX_LADDER_RIGHT)
	{
		//TranslateSprite(THIS, 0, 1 << delta_time);
		SetPlayerState(PLAYER_STATE_IDLE);
	}
	if (KEY_TICKED(J_A)) Jump();
	// can't catch on a ladder
	//if ((keys & J_UP) && (keys & J_B)) CatchSpirit();
	if (KEY_TICKED(J_B)) attack_function();
	if (KEY_TICKED(J_SELECT)) ChangeWeapon();
}
void UpdatePlatform(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
	PlayerData* data = (PlayerData*)THIS->custom_data;
	if (KEY_PRESSED(J_RIGHT) || KEY_PRESSED(J_LEFT)) {
		SetSpriteAnim(THIS, anim_walk, WALK_ANIM_SPEED);
	} else {
		SetSpriteAnim(THIS, anim_walk_idle, DEFAULT_ANIM_SPEED);
	}
	ApplyMovementX(THIS, THIS_IDX);
	ApplyGravity(THIS, THIS_IDX);
	if (KEY_TICKED(J_A)) {
		THIS->y = THIS->y - 1u;
		Jump();
	}
	// can we catch on a platform?
	if ((keys & J_UP) && (keys & J_B)) CatchSpirit();
	else if (KEY_TICKED(J_B)) attack_function();
	if (KEY_TICKED(J_SELECT)) ChangeWeapon();
	CheckOnPlatform();
}
void UpdateHit(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
	PlayerData* data = (PlayerData*)THIS->custom_data;
	ApplyGravity(THIS, THIS_IDX);
	//if (FLAG_CHECK(data->flags, pDeadFlag)) return;
	EMU_printf("SpritePlayer::%s frame %u/%u\n", __func__, THIS->anim_frame, GetLastAnimFrame());
	if (THIS->anim_frame == GetLastAnimFrame() || anim_hit_counter == 0) {
		if (GetPreviousPlayerState() != PLAYER_STATE_HIT) {
			//EMU_printf("SpritePlayer::%s set previous player state\n", __func__);
			SetPlayerState(GetPreviousPlayerState());
		} else {
			SetPlayerState(PLAYER_STATE_IDLE);
		}
	}
}
void UpdateDie(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
	PlayerData* data = (PlayerData*)THIS->custom_data;
	accel_x = 0;
	ApplyGravity(THIS, THIS_IDX);
	// update global dead variable once animation has completed
	if (THIS->anim_frame == GetLastAnimFrame()-1) {
		g_player_dead = true;
	}
}
void UpdateDrown(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
	PlayerData* data = (PlayerData*)THIS->custom_data;
	accel_x = 0;
	// update global dead variable once animation has compelted
	if (THIS->anim_frame == GetLastAnimFrame()) {
		//FLAG_SET(data->flags, pDeadFlag);
		g_player_dead = true;
	}
}	
void UpdateTimeUp(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
}
void UpdateVictory(void) {
	//EMU_printf("SpritePlayer::%s\n", __func__);
	PlayerData* data = (PlayerData*)THIS->custom_data;
	accel_x = accel_y = 0;
	// update g_level_complete variable once animation has compelted
	if (THIS->anim_frame == GetLastAnimFrame()) {
		g_level_complete = true;
	}
}

// update hooks
typedef void (*update_hook_t)(void);
const update_hook_t const update_hooks[N_PLAYER_STATE] = {
	[PLAYER_STATE_SPAWN]		= UpdateSpawn,
	[PLAYER_STATE_IDLE]         = UpdateIdle, 
	[PLAYER_STATE_WALK]	 	    = UpdateWalking,
	[PLAYER_STATE_JUMP]      	= UpdateJumping,
	[PLAYER_STATE_FALL]      	= UpdateFalling,
	[PLAYER_STATE_ATTACK] 		= UpdateAttacking,
	[PLAYER_STATE_CATCH]		= UpdateCatching,
	[PLAYER_STATE_CLIMB]		= UpdateClimbing,
	[PLAYER_STATE_PLATFORM]		= UpdatePlatform,
	[PLAYER_STATE_HIT]			= UpdateHit,
	[PLAYER_STATE_DIE]			= UpdateDie,
	[PLAYER_STATE_DROWN]		= UpdateDrown,
	[PLAYER_STATE_TIMEUP]		= UpdateTimeUp,
	[PLAYER_STATE_VICTORY]     	= UpdateVictory
};

void UPDATE() {
	update_hooks[curPlayerState]();
	PlayerData* data = (PlayerData*)THIS->custom_data;

	// invincible
	if (FLAG_CHECK(data->flags, pInvincibleFlag)) {
		SetVisible(THIS, visible_skip++);
		if (visible_skip > 3) visible_skip = 0;
		if (invincible_secs > 0) {
			invincible_ticks++;
			if (invincible_ticks == 25) {
				invincible_ticks = 0;
				invincible_secs--;
			}
		} else {
			FLAG_CLEAR(data->flags, pInvincibleFlag);
			SetVisible(THIS, true);
		}
	}

	// hit
	if (anim_hit_counter > 0) {
		anim_hit_ticks++;
		if (anim_hit_ticks == 25) {
			anim_hit_ticks = 0;
			anim_hit_counter--;
		}
	}

	// timeup
	if (FLAG_CHECK(data->flags, pTimeUpFlag)) {
		if (data->lives > 0 && data->lives < UCHAR_MAX) data->lives--;
		Hud_Update();
		if (data->lives <= 0) { 
			//SetState(StateGameOver);
			//HIDE_WIN;
			g_player_dead = true;
		}
		SetState(StateTimeUp);
		HIDE_WIN;
	}

	// use / recharge magix
	//if (magix_cooldown) {
	//	magix_cooldown--;
	//} else if (magix_recharge) {
	//	magix_recharge--;
	//}
	//if (!magix_recharge && data->magix < MAGIX_FULL) {
	//	data->magix++;
	//	magix_recharge = MAGIX_RECHARGE_TIME;
	//}

	// check enemy sprite collision - item colission is in each item sprite
	for (UINT8 i = 0u; i != sprite_manager_updatables[0]; ++i) {
		Sprite* spr = sprite_manager_sprites[sprite_manager_updatables[i + 1u]];
		if (spr->type == SpriteSlime || spr->type == SpriteBat || spr->type == SpriteRockard
			|| spr->type == SpriteRocklette) {
			if (CheckCollision(THIS, spr) && !(FLAG_CHECK(data->flags, pInvincibleFlag))) {
				//EMU_printf("SpritePlayer::player x:%d,y%d collided with enemy: x:%d,y:%d\n", THIS->x, THIS->y, spr->x, spr->y);
				Hit();
			}
		}
		if (spr->type == SpritePlatform) {
			if (CheckCollision(THIS, spr) && !(FLAG_CHECK(data->flags, pOnPlatformFlag))) {
				if (PlayerIsOnTopOfPlatform(THIS, spr) && !(GetPlayerState() == PLAYER_STATE_DROWN)) {
					FLAG_SET(data->flags, pOnPlatformFlag);
					FLAG_SET(data->flags, pGroundedFlag);
					accel_x = accel_y = 0;
					SetPlayerState(PLAYER_STATE_PLATFORM);
				}
			}
			// If already on platform, check if player has stepped off
			if (FLAG_CHECK(data->flags, pOnPlatformFlag)) {
				if (!PlayerIsOnTopOfPlatform(THIS, spr)) {
					FLAG_CLEAR(data->flags, pOnPlatformFlag);
				}
			}
		}
		if (spr->type == SpritePortal) {
			if (CheckCollision(THIS, spr) && FLAG_CHECK(data->flags, pCaughtSpiritFlag) && THIS->x > 50) {
				//EMU_printf("SpritePlayer::player x:%d,y%d collided with portal: x:%d,y:%d\n", THIS->x, THIS->y, spr->x, spr->y);
				THIS->x = spr->x-8;
				SetPlayerState(PLAYER_STATE_VICTORY);
				g_level_complete = true;
			}
		}
	}

	prev_keys = keys;

}

void DESTROY() {

}




