#ifndef STATEGAME_H_
#define STATEGAME_H_

#include <gbdk/platform.h>
#include "Sprite.h"
#include "GameTypes.h"

#define TILE_INDEX_LADDER_LEFT          28
#define TILE_INDEX_LADDER_RIGHT         29
#define TILE_INDEX_WATER_1              30
#define TILE_INDEX_WATER_2              31
#define TILE_INDEX_WATER_3              32
#define TILE_INDEX_SPIKE_UP             33
#define TILE_INDEX_SPIKE_DOWN           34
#define TILE_INDEX_PORTAL_TOPLEFT       35
#define TILE_INDEX_PORTAL_TOPRIGHT      36
#define TILE_INDEX_PORTAL_BOTTOMLEFT    37
#define TILE_INDEX_PORTAL_BOTTOMRIGHT   38
#define TILE_INDEX_BG1                  39
#define TILE_INDEX_BG2                  70

/* unique_ids of map entities cleared this level visit (collectables + defeated enemies). */
#define MAX_CLEARED_ENTITIES 32

UINT8 IsMapEntityCleared(Sprite* spr) BANKED;
void ClearMapEntity(Sprite* spr) BANKED;

UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* collectable, ItemType itype) BANKED;

#endif
