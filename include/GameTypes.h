#ifndef GAMETYPES_H_
#define GAMETYPES_H_

#include <stdbool.h>

#define UCHAR_MAX 255
#define MAX_LIVES 5
#define MAX_LEVEL 1

extern UINT16	g_player_score;
extern UINT8	g_level_current;
//extern UINT8	g_player_lives;
extern bool		g_player_dead;

typedef struct {
	UINT8 x;
	UINT8 y;
} ItemPos;

typedef enum {
	ITEM_PORTAL,
	ITEM_MANA,
	ITEM_SPIRIT,
	ITEM_COIN,
	ITEM_HEALTH,
	ITEM_BULLET
} ItemType;

#endif // GAMETYPES_H