#ifndef SPRITEPLAYER_H
#define SPRITEPLAYER_H

#include <stdbool.h>
#include "main.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "ZGBMain.h"
#include "ZGBUtils.h"


#define pGroundedFlag			0	/* player is on the ground, i.e. not jumping */
#define pDeadFlag				1	/* player has died, drowned etc. */
#define pTimeUpFlag				2	/* player has run out of time */
#define	pInvincibleFlag			3	/* player is invincible, i.e. cannot lose health*/
#define pAnimPlayingFlag		4	/* player animation is playing frames and needs to complete all of them */
#define pCaughtSpiritFlag		5	/* player has caught the sprit */
#define pOnPlatformFlag			6	/* player is on a platform */
#define pOnLadderFlag			7	/* player is on a ladder */

#define pWeaponKnife			0
#define pWeaponMagix			1

#define MAX_WEAPON				1

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
	PLAYER_STATE_CATCH,
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

typedef struct {
	UINT8 flags;
	UINT8 start_x;
	UINT8 start_y;
	UINT8 lives;	// number of lives player has
	UINT8 weapon;	// weapon player has currently selected
	UINT8 spirits;	// number of spirits remaining to be c
	UINT8 magix;	// number of magix remaining
} PlayerData;

void SetPlayerState(PlayerState state) BANKED;
PlayerState GetPlayerState(void) BANKED;
PlayerState GetPreviousPlayerState(void) BANKED;
void SetPreviousPlayerState(void) BANKED;
bool GetPlayerStateEquals(PlayerState ps) BANKED;
bool PlayerIsOnTopOfPlatform(Sprite* player, Sprite* platform) BANKED;

#endif // SPRITEPLAYER_H