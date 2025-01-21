#ifndef GAMETYPES_H_
#define GAMETYPES_H_

#define MAX_LIVES 6
#define MAX_LEVEL 1

extern UINT16 g_player_score;
extern UINT8 g_level_current;

typedef struct {
	UINT8 x;
	UINT8 y;
} ItemPos;

typedef enum {
	ITEM_PORTAL,
	ITEM_COIN,
	ITEM_BULLET,
	ITEM_SPIRIT
} ItemType;

#endif // GAMETYPES_H