#ifndef SPRITEPLAYER_H
#define SPRITEPLAYER_H

#include "main.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "ZGBMain.h"
#include "ZGBUtils.h"

#include "GameTypes.h"
#include "Hud.h"

#define pGroundedFlag		0	/* player is on the ground, i.e. not jumping */
#define pTimeUpFlag			1	/* player has run out of time */
#define	pInvincibleFlag		2	/* player is invincible, i.e. cannot lose a life */
#define pAnimPlayingFlag	3	/* player animation is playing frames and needs to complete all of them */
#define pHasSpiritFlag		4	/* player has caught the sprit */

#define FLAG_SET(var, flag) 	((var) |=  (1 << (flag))) 
#define FLAG_CLEAR(var, flag) 	((var) &= ~(1 << (flag))) 
#define FLAG_CHECK(var, flag) 	((var) &   (1 << (flag))) 
#define FLAG_TOGGLE(var, flag) 	((var) ^=  (1 << (flag)))

#define DEFAULT_ANIM_SPEED 		10
#define WALK_ANIM_SPEED 		10
#define HIT_ANIM_SPEED 			254
#define DIE_ANIM_SPEED 			254
#define DISAPPEAR_ANIM_SPEED 	10
#define VICTORY_ANIM_SPEED 		8

#define X_SPEED_MAX         	100
#define X_SPEED_INC         	10
#define X_DAMPENING         	5
#define Y_SPEED_MAX				150
#define Y_GRAVITY				10
#define Y_JUMP_HEIGHT			200

typedef enum {
	PLAYER_STATE_IDLE,
	PLAYER_STATE_WALKING,
	PLAYER_STATE_JUMPING,
	PLAYER_STATE_FALLING,
	PLAYER_STATE_PUSHING,
	PLAYER_STATE_ATTACKING,
	PLAYER_STATE_HIT,
	PLAYER_STATE_DIE,
	PLAYER_STATE_TIMEOVER,
	PLAYER_STATE_APPEAR,
	PLAYER_STATE_DISAPPEAR,
	PLAYER_STATE_DROWNING
} PlayerState;

typedef enum {
    NORMAL,
	WALK,
	WALK_IDLE,
	JUMP,
	FALL,
	PUSH,
    ATTACK,
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

#endif // SPRITEPLAYER_H