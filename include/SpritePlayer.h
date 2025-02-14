#ifndef SPRITEPLAYER_H
#define SPRITEPLAYER_H

#include <stdbool.h>
#include "main.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "ZGBMain.h"
#include "ZGBUtils.h"

#include "GameTypes.h"
#include "Hud.h"

#define pGroundedFlag			0	/* player is on the ground, i.e. not jumping */
#define pDeadFlag				1	/* player has died, drowned etc. */
#define pTimeUpFlag				2	/* player has run out of time */
#define	pInvincibleFlag			3	/* player is invincible, i.e. cannot lose a life */
#define pAnimPlayingFlag		4	/* player animation is playing frames and needs to complete all of them */
#define pCaughtSpiritFlag		5	/* player has caught the sprit */
#define pOnPlatformFlag			6	/* player is on a platform */
#define pOnLadder				7	/* player is on a ladder */

#define FLAG_SET(var, flag) 	((var) |=  (1 << (flag))) 
#define FLAG_CLEAR(var, flag) 	((var) &= ~(1 << (flag))) 
#define FLAG_CHECK(var, flag) 	((var) &   (1 << (flag))) 
#define FLAG_TOGGLE(var, flag) 	((var) ^=  (1 << (flag)))

#define DEFAULT_ANIM_SPEED 		10
#define WALK_ANIM_SPEED 		10
#define HIT_ANIM_SPEED 			254
#define DIE_ANIM_SPEED 			254
#define DISAPPEAR_ANIM_SPEED 	10
#define DROWN_ANIM_SPEED		10
#define VICTORY_ANIM_SPEED 		8

#define X_SPEED_MAX         	100
#define X_SPEED_INC         	10
#define X_DAMPENING         	5
#define Y_SPEED_MAX				150
#define Y_GRAVITY				10
#define Y_JUMP_HEIGHT			200
#define MAGIX_RECHARGE_TIME		60
#define MAGIX_COOLDOWN_TIME		60
#define MAGIX_FULL				12

typedef enum {
	PLAYER_STATE_SPAWN,
	PLAYER_STATE_IDLE,
	PLAYER_STATE_WALK,
	PLAYER_STATE_JUMP,
	PLAYER_STATE_FALL,
	PLAYER_STATE_ATTACK,
	PLAYER_STATE_CLIMB,
	PLAYER_STATE_PLATFORM,
	PLAYER_STATE_HIT,
	PLAYER_STATE_DIE,
	PLAYER_STATE_DROWN,
	PLAYER_STATE_TIMEUP,
	PLAYER_STATE_APPEAR,
	PLAYER_STATE_DISAPPEAR,
	PLAYER_STATE_VICTORY,
	N_PLAYER_STATE
} PlayerState;

typedef enum {
    NORMAL,
	WALK,
	WALK_IDLE,
	JUMP,
	FALL,
	PUSH,
    ATTACK,
	CLIMB,
	CLIMB_IDLE,
    HIT,
	DIE,
	APPEAR,
	DISAPPEAR,
	DROWN,
	VICTORY
} AnimationState;

typedef struct {
	UINT8 flags;
	UINT8 start_x;
	UINT8 start_y;
	UINT8 lives;
	UINT8 coins;
	UINT8 magix;
} PlayerData;

void SetPlayerState(PlayerState state) BANKED;
PlayerState GetPlayerState(void) BANKED;
bool GetPlayerStateEquals(PlayerState ps) BANKED;
//AnimationState GetAnimationState(void) BANKED;
//void SetAnimationState(AnimationState state) BANKED;

#endif // SPRITEPLAYER_H