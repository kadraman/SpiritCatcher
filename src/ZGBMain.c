#include "ZGBMain.h"
#include "Math.h"
#include "GlobalVars.h"

UINT16 g_player_score = 0;
UINT8 next_state = StateSplash;

UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	if (current_state == StateLevel1 || current_state == StateLevel2) {
		*tile = 0;
		switch (*tile_ptr) {
			// items
			//case 51: 	return SpriteCoin;
			//case 52:	return SpriteAmmo;
			// enemies
			case 20:	return SpriteEnemy1;
			//case 72:	return SpriteEnemy2;
			//case 125: 	return SpriteFlag;
			default: break;
		}
		*tile = *tile_ptr;
	}
	return 255u;
}