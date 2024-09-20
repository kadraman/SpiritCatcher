#ifndef SPRITEPLAYER_H
#define SPRITEPLAYER_H

#include "main.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "ZGBMain.h"
#include "GlobalVars.h"
#include "zgb_utils.h"
#include "Hud.h"

#define DEBUG_CONTROLS 0

#define GROUNDED_BIT 7u
#define DOUBLE_JUMP_BIT 6u
#define DEFAULT_ANIM_SPEED 10
#define WALK_ANIM_SPEED 10
#define HIT_ANIM_SPEED 10
#define VICTORY_ANIM_SPEED 8

typedef enum {
	PLAYER_STATE_IDLE,
	PLAYER_STATE_WALKING,
	PLAYER_STATE_JUMPING,
	PLAYER_STATE_CLIMBING,
	PLAYER_STATE_ATTACKING,
	PLAYER_STATE_HIT
} PlayerState;

typedef enum {
    NORMAL,
	WALK,
	WALK_IDLE,
	JUMP,
	FALL,
	CLIMB,
	CLIMB_IDLE,
    ATTACK,
    HIT,
	DEAD,
	VICTORY
} AnimationState;

typedef struct {
	UINT8 flags;
	INT8 jump;
	UINT8 lives;
	UINT8 bullets;
	UINT8 invincible;
} PlayerData;

#endif // SPRITEPLAYER_H