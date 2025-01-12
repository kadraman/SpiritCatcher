#ifndef GLOBALVARS_H_
#define GLOBALVARS_H_

#define MAX_LIVES 6
#define MAX_LEVEL 1

#define TILE_ITEM_SPIKE	200
#define TILE_ITEM_COIN	201
#define TILE_ITEM_BOX	202
#define TILE_ITEM_FLAG	203

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

#endif