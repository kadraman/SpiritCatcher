#ifndef GAMETYPES_H_
#define GAMETYPES_H_

#include <stdbool.h>

#define UCHAR_MAX 255
#define MAX_LIVES 5
#define MAX_LEVEL 1	// Not used - remove?

extern UINT8    g_stage_current;
extern UINT8	g_level_current;
extern UINT8 	g_next_portal;		// Not used - remove?
extern UINT16	g_player_score;		// Not yet used
extern UINT8	g_player_lives;
extern bool		g_player_dead;
extern bool		g_game_reset;

typedef struct {
	UINT8 x;
	UINT8 y;
} ItemPos;		// Note used - remove?

typedef enum {
	ITEM_PORTAL,
	ITEM_MANA,
	ITEM_SPIRIT,
	ITEM_COIN,
	ITEM_HEALTH,
	ITEM_BULLET
} ItemType;

#endif // GAMETYPES_H