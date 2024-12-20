#include "ZGBMain.h"
#include "Math.h"
#include "StateGame.h"
#include "GlobalVars.h"

UINT16 g_player_score = 0;

#ifdef NDEBUG
UINT8 next_state = StateSplash;
#else
UINT8 next_state = StateGame;
#endif

UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	if (current_state == StateGame) {
		*tile = 0;
		switch (*tile_ptr) {
			case TILE_INDEX_PLAYER: 
				*tile = TILE_INDEX_BG; 
				return 255u;
			case TILE_INDEX_GOAL: 
				*tile = TILE_INDEX_BG; 
				return SpritePortal;
			case TILE_INDEX_COIN: 
				*tile = TILE_INDEX_BG; 
				return SpriteCoin;
			case TILE_INDEX_SPIRIT1: 
				*tile = TILE_INDEX_BG; 
				return SpriteEnemy1;
			case TILE_INDEX_SPIRIT2: 
				*tile = TILE_INDEX_BG; 
				return SpriteSpirit;
			case TILE_INDEX_BAT: 
				*tile = TILE_INDEX_BG; 
				return 255u;
			case TILE_INDEX_TOADSTALL: 
				*tile = TILE_INDEX_BG; 
				return 255u;
			default: break;
		}
		*tile = *tile_ptr;
	}
	return 255u;
}

/*UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	if(current_state == StateLevel1 || current_state == StateLevel2) {
		if((0xffu - *tile_ptr) < N_SPRITE_TYPES) {
			*tile = 0;
			return 0xffu - *tile_ptr;
		}
		*tile = *tile_ptr;
	}

	return 0xffu;
}*/