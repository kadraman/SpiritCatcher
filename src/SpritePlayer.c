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

#include "StateGame.h"
#include "SpritePlayer.h"

// player animations - the first number indicates the number of frames
const UINT8 anim_idle[] = {3, 0, 1, 2};		
const UINT8 anim_idle_attack[] = {1, 20};		
const UINT8 anim_walk[] = {4, 1, 3, 4, 5};
const UINT8 anim_walk_attack[] = {1, 20};
const UINT8 anim_jump[] = {2, 6, 7};
const UINT8 anim_fall[] = {1, 8};
const UINT8 anim_jump_attack[] = {1, 20};
const UINT8 anim_attack[] = {2, 19, 20};
const UINT8 anim_climb[] = {2, 21, 22};
const UINT8 anim_hit[] = {4, 9, 10, 9, 10};
const UINT8 anim_die[] = {14, 9, 10, 9, 10, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12};
const UINT8 anim_appear[] = {3, 15, 14, 13};
const UINT8 anim_disappear[] = {5, 14, 14, 13, 14, 15};
const UINT8 anim_drown[] = {10, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18};
//const UINT8 anim_victory[] = {2, 21, 22}; // TBD


Sprite* player_sprite = 0;
Sprite* attack1_sprite = 0;
extern Sprite* attack_particle;
extern UINT8 start_x, start_y;
extern INT16 scroll_x, scroll_y;
extern INT16 min_x, max_x, min_y, max_y;
PlayerState curPlayerState, prevPlayerState;
AnimationState lastAnimState, currentAnimState;

INT16 accel_y;
INT16 accel_x;
UINT16 x_inc;
UINT16 y_inc;
UINT8 reset_x;
UINT8 reset_y;
UINT8 magix_cooldown;
UINT8 magix_recharge;
UINT8 pause_secs;
UINT8 pause_ticks;
UINT8 invincible_secs;
UINT8 invincible_ticks;
UINT8 anim_hit_counter;
UINT8 player_spawned;
UINT8 visible_skip;
UINT8 tile_collision;

extern UINT16 timer_countdown;
extern UINT16 level_max_time;
extern UINT8 level_complete;
extern UINT16 level_width;
extern UINT16 level_height;

void SetPlayerState(PlayerState state) BANKED {
	prevPlayerState = curPlayerState;
	curPlayerState = state;
}

PlayerState GetPlayerState(void) BANKED {
	return curPlayerState;
}

bool GetPlayerStateEquals(PlayerState ps) BANKED {
	return curPlayerState == ps;
}

void SetAnimationState(AnimationState state) BANKED {
	lastAnimState = currentAnimState;
	currentAnimState = state;
	switch (state) {
		case WALK:    		SetSpriteAnim(THIS, anim_walk, WALK_ANIM_SPEED); break;
		case WALK_IDLE:    	SetSpriteAnim(THIS, anim_idle, DEFAULT_ANIM_SPEED);	break;
		case JUMP:    		SetSpriteAnim(THIS, anim_jump, DEFAULT_ANIM_SPEED); break;
		case FALL:    		SetSpriteAnim(THIS, anim_fall, DEFAULT_ANIM_SPEED); break;
		case ATTACK:		SetSpriteAnim(THIS, anim_attack, DEFAULT_ANIM_SPEED); break;	
		case CLIMB:			SetSpriteAnim(THIS, anim_climb, DEFAULT_ANIM_SPEED); break;
		case HIT:			SetSpriteAnim(THIS, anim_hit, HIT_ANIM_SPEED); break;
		case DROWN:			SetSpriteAnim(THIS, anim_drown, DROWN_ANIM_SPEED); break;
		case DIE:    		SetSpriteAnim(THIS, anim_die, DIE_ANIM_SPEED); break;
		case APPEAR:		SetSpriteAnim(THIS, anim_appear, DISAPPEAR_ANIM_SPEED); break;
		case DISAPPEAR:		SetSpriteAnim(THIS, anim_disappear, DISAPPEAR_ANIM_SPEED); break;
		//case VICTORY: 		SetSpriteAnim(THIS, anim_victory, VICTORY_ANIM_SPEED); break;
	}
}

AnimationState GetAnimationState(void) BANKED {
	return currentAnimState;
}

UINT8 GetLastAnimFrame() BANKED {
	return VECTOR_LEN(THIS->anim_data)-1;
}

void UpdateAttackPos() {
	attack1_sprite->mirror = THIS->mirror;
	if(THIS->mirror == V_MIRROR) 
		attack1_sprite->x = THIS->x - 14u;
	else
		attack1_sprite->x = THIS->x + 14u; 
	attack1_sprite->y = THIS->y + 2u;
}

void Hit(Sprite* sprite, UINT8 idx) {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	if (FLAG_CHECK(data->flags, pInvincibleFlag)) return;
	if (--g_player_lives <= 0) {
		SetPlayerState(PLAYER_STATE_DIE);
		PlayFx(CHANNEL_1, 10, 0x5b, 0x7f, 0xf7, 0x15, 0x86);
		SetAnimationState(DIE);
		FLAG_SET(data->flags, pAnimPlayingFlag);
		//invincible_secs = 10;
	} else {
		SetPlayerState(PLAYER_STATE_HIT);
		PlayFx(CHANNEL_1, 10, 0x5b, 0x7f, 0xf7, 0x15, 0x86);
		SetAnimationState(HIT);
		FLAG_SET(data->flags, pAnimPlayingFlag);
		FLAG_SET(data->flags, pInvincibleFlag);
		invincible_secs = 3;
	}
	Hud_Update();	
}

void Drown(Sprite* sprite, UINT8 idx) {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	if (GetPlayerState() == PLAYER_STATE_DROWNING) return;
	SetPlayerState(PLAYER_STATE_DROWNING);
	PlayFx(CHANNEL_1, 10, 0x5b, 0x7f, 0xf7, 0x15, 0x86);
	SetAnimationState(DROWN);
	FLAG_SET(data->flags, pAnimPlayingFlag);
	//THIS->y = THIS->y + 1;
	//invincible_secs = 10;
}

void BridgeDissolve() {
	UINT8 adjacent_tile = GetScrollTile((THIS->x + 8) >> 3, (THIS->y + 8) >> 3);
	EMU_printf("SpritePlayer::BridgeDissolve: player on tile: %d\n", adjacent_tile);

	//if(adjacent_tile == 27u) {
	//	set_bkg_tile_xy((THIS->x + i) >> 3, (THIS->y + j) >> 3, 1); //
	//}
}

void Collected(Sprite* sprite, ItemType itype, UINT8 idx) {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	switch (itype) {
		case ITEM_COIN:
			PlayFx(CHANNEL_1, 10, 0x5b, 0x7f, 0xf7, 0x15, 0x86);
			data->coins++;
			break;
		case ITEM_SPIRIT:
			FLAG_SET(data->flags, pCaughtSpiritFlag);
			PlayFx(CHANNEL_1, 10, 0x5b, 0x7f, 0xf7, 0x15, 0x86);
			break;
		default:
			break;
	}
	Hud_Update();
}

void Jump() {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	FLAG_CLEAR(data->flags, pGroundedFlag);
	FLAG_CLEAR(data->flags, pOnPlatformFlag);
	PlayFx(CHANNEL_1, 5, 0x17, 0x9f, 0xf3, 0xc9, 0xc4);
	SetPlayerState(PLAYER_STATE_JUMPING);
	SetAnimationState(JUMP);
	accel_y = -Y_JUMP_HEIGHT;
}

void Attack() {
	SetPlayerState(PLAYER_STATE_ATTACKING);
	SetAnimationState(ATTACK);
	attack1_sprite = SpriteManagerAdd(SpriteAttack1, THIS->x, THIS->y);
	UpdateAttackPos();
	PlayFx(CHANNEL_4, 20, 0x0d, 0xff, 0x7d, 0xc0);
}

void Magix() {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	// no magix left
	if (data->magix == 0) return;
	SetPlayerState(PLAYER_STATE_ATTACKING);
	SetAnimationState(ATTACK);
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
	magix_cooldown = MAGIX_COOLDOWN_TIME;
	magix_recharge = MAGIX_RECHARGE_TIME;
}

void CheckCollisionTile(Sprite* sprite, UINT8 idx) {
	switch (tile_collision) {
		case TILE_INDEX_SPIKE_UP:
		case TILE_INDEX_SPIKE_DOWN:
			Hit(sprite, idx);
			break;
		case TILE_INDEX_WATER_1:
		case TILE_INDEX_WATER_2:
		case TILE_INDEX_WATER_3:
			Drown(sprite, idx);
			break;
		case TILE_INDEX_BRIDGE_1:
		case TILE_INDEX_BRIDGE_2:
		case TILE_INDEX_BRIDGE_3:
		case TILE_INDEX_BRIDGE_4:
		case TILE_INDEX_BRIDGE_5:
		case TILE_INDEX_BRIDGE_6:
			BridgeDissolve();
			break;
		default:	
			// ignore
			break;
	}
}

void AddDamping(Sprite* sprite, UINT8 idx) {
	if (accel_x > 0) {
		accel_x -= X_DAMPENING;
		x_inc = accel_x >> 6;
		tile_collision = TranslateSprite(sprite, x_inc, 0);
	} else if (accel_x < 0) {
		accel_x += X_DAMPENING;
		x_inc = abs(accel_x) >> 6;
		tile_collision = TranslateSprite(sprite, -x_inc, 0);
	}
	CheckCollisionTile(sprite, idx);
}

void HandleInput(Sprite* sprite, UINT8 idx) {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	//if (GetPlayerState() == PLAYER_STATE_HIT || GetPlayerState() == PLAYER_STATE_DROWNING || GetPlayerState() == PLAYER_STATE_DIE) return;
	
	// player movement
	if (KEY_PRESSED(J_RIGHT)) {
		if (accel_x < (X_SPEED_MAX-X_SPEED_INC)) accel_x += X_SPEED_INC;	
		x_inc = accel_x >> 6;	
		tile_collision = TranslateSprite(sprite, x_inc, 0);
		CheckCollisionTile(sprite, idx);
		THIS->mirror = NO_MIRROR;
		if (GetPlayerState() != PLAYER_STATE_JUMPING) {
			SetPlayerState(PLAYER_STATE_WALKING);
			SetAnimationState(WALK);
		}
	} else if (KEY_PRESSED(J_LEFT)) {
		if (accel_x > -(X_SPEED_MAX-X_SPEED_INC)) accel_x -= X_SPEED_INC;
		x_inc = abs(accel_x) >> 6;
		if (sprite->x - x_inc < min_x) {
			x_inc = accel_x = 0;
		}
		tile_collision = TranslateSprite(sprite, -x_inc, 0);
		CheckCollisionTile(sprite, idx);
		THIS->mirror = V_MIRROR;
		if (GetPlayerState() != PLAYER_STATE_JUMPING) {
			SetPlayerState(PLAYER_STATE_WALKING);
			SetAnimationState(WALK);
		}
	} else {
		AddDamping(THIS, THIS_IDX);
	}

	// climbing ladders
	if (KEY_PRESSED(J_UP)) {
		UINT8 adjacent_tile = GetScrollTile((THIS->x + 4u) >> 3, (THIS->y + 4u) >> 3);
		EMU_printf("SpritePlayer::PRESSED_UP: adjacent_tile: %d\n", adjacent_tile);
		if (adjacent_tile == TILE_INDEX_LADDER_LEFT || adjacent_tile == TILE_INDEX_LADDER_RIGHT) {
			THIS->x = (((THIS->x)>> 3) << 3) + 4;
			THIS->y = THIS->y - 1u;
			accel_y = 0;
			SetPlayerState(PLAYER_STATE_CLIMBING);
			SetAnimationState(CLIMB);
		}
	} else if (KEY_PRESSED(J_DOWN)) {
		UINT8 adjacent_tile = GetScrollTile((THIS->x) >> 3, (THIS ->y + 16u) >> 3);
		EMU_printf("SpritePlayer::PRESSED_DOWN: adjacent_tile: %d\n", adjacent_tile);
		if(adjacent_tile == TILE_INDEX_LADDER_LEFT || adjacent_tile == TILE_INDEX_LADDER_RIGHT) {
			THIS->x = (((THIS->x)>> 3) << 3) + 4;
			THIS->y = THIS->y + 1u;
			accel_y = 0;
			SetPlayerState(PLAYER_STATE_CLIMBING);
			SetAnimationState(CLIMB);
		}
	} else {
		// AddDamping(THIS, THIS_IDX);
	}

	// jumping
	if (KEY_TICKED(J_A) && (FLAG_CHECK(data->flags, pGroundedFlag) || FLAG_CHECK(data->flags, pOnPlatformFlag))) {
		FLAG_CLEAR(data->flags, pGroundedFlag);
		FLAG_CLEAR(data->flags, pOnPlatformFlag);
		PlayFx(CHANNEL_1, 5, 0x17, 0x9f, 0xf3, 0xc9, 0xc4);
		SetPlayerState(PLAYER_STATE_JUMPING);
		SetAnimationState(JUMP);
		accel_y = -Y_JUMP_HEIGHT;
	} else {
		// check if now FALLING?
		if ((accel_y >> 6) > 1) {
			SetAnimationState(FALL);
		}
	}

	// attack or capture spirit
	if (KEY_TICKED(J_B) && (GetPlayerState() != PLAYER_STATE_ATTACKING && GetPlayerState() != PLAYER_STATE_HIT && GetPlayerState() != PLAYER_STATE_DROWNING && GetPlayerState() != PLAYER_STATE_DIE)) {
		if (KEY_PRESSED(J_UP)) {
			// TODO: capture spirit
		} else {
			//Attack();
			if (!magix_cooldown) {
				Magix();
			}
		}
	}
	//
	//if (keys == 0) AddDamping(THIS, THIS_IDX);

	// nothing happening lets revert to idle state
	if (keys == 0 && !(GetPlayerState() == PLAYER_STATE_JUMPING || GetPlayerState() == PLAYER_STATE_ATTACKING)) {
		SetAnimationState(WALK_IDLE);
	}
	
	// magix cooldown/recharge
	if (GetPlayerState() != PLAYER_STATE_HIT) {
		if (magix_cooldown) {
			magix_cooldown--;
		} else if (magix_recharge) {
			magix_recharge--;
		}
		if (!magix_recharge && data->magix < MAGIX_FULL) {
			data->magix++;
			magix_recharge = MAGIX_RECHARGE_TIME;
		}
	}
}

//

void START() {
	memset((PlayerData*)(THIS->custom_data), 0, CUSTOM_DATA_SIZE);
	PlayerData* data = (PlayerData*)THIS->custom_data;
	g_player_dead = false;
	player_sprite = THIS;
	data->start_x = THIS->x;
	data->start_y = THIS->y;
	FLAG_SET(data->flags, pGroundedFlag);
	//data->lives = MAX_LIVES;
	data->magix = 12;
	data->coins = 0;
	//curPlayerState = PLAYER_STATE_IDLE;
	accel_y = 0;
	accel_x = 0;
	magix_cooldown = 0;
	player_spawned = true;
	level_complete = false;
	scroll_target = THIS;
	reset_x = 20;
	reset_y = 80;
	attack1_sprite = 0;
	anim_hit_counter = 0;
	pause_secs = 0;
	pause_ticks = 0;
	visible_skip = 0;
	tile_collision = 0;
	SetPlayerState(PLAYER_STATE_APPEAR);
	SetAnimationState(APPEAR);
	EMU_printf("SpritePlayer::START: flags: %d\n", data->flags);
}

void UPDATE() {
	PlayerData* data = (PlayerData*)THIS->custom_data;
	UINT8 i;
	Sprite* spr;

	// player spawned
	/*if (player_spawned) {
		if (THIS->anim_frame == 2) {
			SetPlayerState(PLAYER_STATE_IDLE);
			SetAnimationState(NORMAL);
			player_spawned = false;
		}
		return;
	}*/

	// pause
	if (pause_secs) {
		pause_ticks++;
		if (pause_ticks == 25) {
			pause_ticks = 0;
			pause_secs--;
		}
		return;
	}

	// invincible
	if (FLAG_CHECK(data->flags, pInvincibleFlag)) {
		SetVisible(THIS, visible_skip++);
		if (visible_skip > 3) visible_skip = 0;
	}
	if (invincible_secs) {
		invincible_ticks++;
		if (invincible_ticks == 25) {
			invincible_ticks = 0;
			invincible_secs--;
		}
	} else {
		FLAG_CLEAR(data->flags, pInvincibleFlag);
		SetVisible(THIS, true);
	}

	// timeup
	if (FLAG_CHECK(data->flags, pTimeUpFlag)) {
		g_player_lives--;
		Hud_Update();
		if (g_player_lives <= 0) { 
			//SetState(StateGameOver);
			//HIDE_WIN;
			g_player_dead = true;
		}
		SetState(StateTimeUp);
		HIDE_WIN;
	}

	// level complete
	if (level_complete) {
		if (THIS->anim_frame == 2) {
			FLAG_CLEAR(data->flags, pAnimPlayingFlag);
			if (g_level_current == MAX_LEVEL) {
				SetState(StateWin);
				HIDE_WIN;
			} else {
				g_level_current++;
				SetState(StateGame);
			}
		}
		return;
	}

	// dead, drowned etc
	if (GetPlayerState() == PLAYER_STATE_DROWNING || GetPlayerState() == PLAYER_STATE_DIE) {
		accel_x = 0;
		// update global dead variable once animation has compelted
		if (THIS->anim_frame == GetLastAnimFrame()) {
			EMU_printf("SpritePlayer::UPDATE: player has died in state: %d\n", GetPlayerState());
			FLAG_SET(data->flags, pDeadFlag);
			g_player_dead = true;
		}
		return;
	}

	// all states
	switch (GetPlayerState()) {
		case PLAYER_STATE_ATTACKING:
			UpdateAttackPos();
			if (THIS->anim_frame == 1) {
				FLAG_CLEAR(data->flags, pAnimPlayingFlag);
				SetAnimationState(lastAnimState);
			}
			break;
		case PLAYER_STATE_JUMPING:
			// check if now FALLING?
			if ((accel_y >> 6) > 1) {
				SetPlayerState(PLAYER_STATE_FALLING);
				SetAnimationState(FALL);
			}
			break;
		case PLAYER_STATE_CLIMBING:
			i = TILE_INDEX_LADDER_LEFT;
			if(KEY_PRESSED(J_UP)) {
				//SetSpriteAnim(THIS, shoot_cooldown ? anim_ladder_moving_cooldown : anim_ladder_moving, 12u);
				tile_collision = TranslateSprite(THIS, 0, -1 << delta_time);
				CheckCollisionTile(THIS, THIS_IDX);
				i = GetScrollTile((THIS->x) >> 3, (THIS->y + 13u) >> 3);
			} else if(KEY_PRESSED(J_DOWN)) {
				//SetSpriteAnim(THIS, shoot_cooldown ? anim_ladder_moving_cooldown : anim_ladder_moving, 12u);
				tile_collision = TranslateSprite(THIS, 0, 1 << delta_time);
				CheckCollisionTile(THIS, THIS_IDX);
				i = GetScrollTile((THIS->x) >> 3, (THIS->y + 14u) >> 3);
			} else {
				//SetSpriteAnim(THIS, shoot_cooldown ? anim_ladder_idle_cooldown : anim_ladder_idle, 12u);
			}
			if(KEY_PRESSED(J_RIGHT)) {
				THIS->mirror = NO_MIRROR;
			} else if(KEY_PRESSED(J_LEFT)) {
				THIS->mirror = V_MIRROR;
			}

			//Check the end of the ladder
			if(i != TILE_INDEX_LADDER_LEFT && i != TILE_INDEX_LADDER_RIGHT)
			{
				//TranslateSprite(THIS, 0, 1 << delta_time);
				SetPlayerState(PLAYER_STATE_WALKING);
				SetAnimationState(WALK_IDLE);
			}

			//Check jumping
			if(KEY_TICKED(J_A)) {
				Jump();
			}
			break;
		case PLAYER_STATE_HIT:
			accel_x = 0;
			if (THIS->anim_frame == 3) {
				FLAG_CLEAR(data->flags, pAnimPlayingFlag);
				SetPlayerState(prevPlayerState);
			}
			break;
		default:
			if (keys == 0 && !FLAG_CHECK(data->flags, pAnimPlayingFlag)) {
				SetAnimationState(WALK_IDLE);
			}
			player_spawned = false;

	}

	HandleInput(THIS, THIS_IDX);

	// simple gravity physics - unless on platform
	if (accel_y < Y_SPEED_MAX && !FLAG_CHECK(data->flags, pOnPlatformFlag)) {
		accel_y += Y_GRAVITY;
	}
	tile_collision = TranslateSprite(THIS, 0, accel_y >> 6);
	//if (!tile_collision && delta_time != 0 && accel_y < Y_SPEED_MAX) { 
		//do another iteration if there is no collision
	//	accel_y += Y_GRAVITY;
	//	tile_collision = TranslateSprite(THIS, 0, accel_y >> 6);
	//}
	if (tile_collision) {
		accel_y = 0;
		if (currentAnimState == JUMP || currentAnimState == FALL) {
			currentAnimState = WALK_IDLE;
		}
		curPlayerState = PLAYER_STATE_IDLE;
		FLAG_SET(data->flags, pGroundedFlag);
		CheckCollisionTile(THIS, THIS_IDX);
	}

	// check enemy sprite collision - item colission is in each item sprite
	for (i = 0u; i != sprite_manager_updatables[0]; ++i) {
		spr = sprite_manager_sprites[sprite_manager_updatables[i + 1u]];
		if (spr->type == SpriteSlime || spr->type == SpriteBat || spr->type == SpriteMushroom) {
			if (CheckCollision(THIS, spr) && !(FLAG_CHECK(data->flags, pInvincibleFlag))) {
				Hit(THIS, THIS_IDX);
			}
		}
		if (spr->type == SpritePlatform) {
			if (CheckCollision(THIS, spr) && !(FLAG_CHECK(data->flags, pOnPlatformFlag))) {
				// player above platform
				if (THIS->y <= spr->y) {
					EMU_printf("SpritePlayer::player x:%d,y%d collided with platform: x:%d,y:%d\n", THIS->x, THIS->y, spr->x, spr->y);
					FLAG_SET(data->flags, pOnPlatformFlag);
					FLAG_SET(data->flags, pGroundedFlag);
					accel_x = accel_y = 0;
					SetPlayerState(PLAYER_STATE_IDLE);
					SetAnimationState(PLAYER_STATE_IDLE);
				}
			}
		}
		if (spr->type == SpritePortal) {
			if (CheckCollision(THIS, spr) && !player_spawned && FLAG_CHECK(data->flags, pCaughtSpiritFlag) && THIS->x > 16) {
				THIS->x = spr->x-8;
				SetAnimationState(DISAPPEAR);
				SetPlayerState(PLAYER_STATE_DISAPPEAR);
				FLAG_SET(data->flags, pAnimPlayingFlag);
				level_complete = true;
				//pause_secs = 5;
			}
		}
	}

}

void DESTROY() {

}




