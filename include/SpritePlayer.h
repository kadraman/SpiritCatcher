#ifndef SPRITEPLAYER_H
#define SPRITEPLAYER_H

#include "main.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "ZGBMain.h"
#include "ZGBUtils.h"

#include "GlobalVars.h"
#include "Hud.h"


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
#define Y_JUMP_HEIGHT			160

typedef enum {
	PLAYER_STATE_IDLE,
	PLAYER_STATE_WALKING,
	PLAYER_STATE_BEFORE_JUMP,
	PLAYER_STATE_JUMPING,
	PLAYER_STATE_DOUBLE_JUMPING,
	PLAYER_STATE_AFTER_JUMP,
	PLAYER_STATE_FALLING,
	PLAYER_STATE_CLIMBING,
	PLAYER_STATE_PUSHING,
	PLAYER_STATE_ATTACKING,
	PLAYER_STATE_HIT,
	PLAYER_STATE_DIE,
	PLAYER_STATE_TIMEOVER,
	PLAYER_STATE_APPEAR,
	PLAYER_STATE_DISAPPEAR
} PlayerState;

typedef enum {
    NORMAL,
	WALK,
	WALK_IDLE,
	BEFORE_JUMP,
	JUMP,
	DOUBLE_JUMP,
	AFTER_JUMP,
	FALL,
	CLIMB,
	CLIMB_IDLE,
	PUSH,
    ATTACK,
    HIT,
	DIE,
	APPEAR,
	DISAPPEAR,
	VICTORY
} AnimationState;

typedef struct {
	UINT8 anim_playing;
	UINT8 start_x;
	UINT8 start_y;
	UINT8 lives;
	UINT8 coins;
	UINT8 knives;
	UINT8 spirits;
	UINT8 timeup;
	UINT8 invincible;
} PlayerData;

#endif // SPRITEPLAYER_H