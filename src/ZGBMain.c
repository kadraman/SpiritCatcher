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

/*UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	if (current_state == StateGame) {
		*tile = 0;
		UINT8 ret_val = 255u;
		switch (*tile_ptr) {
			case TILE_INDEX_PLAYER: 	
				break;
			case TILE_INDEX_GOAL: 
				ret_val = SpritePortal;	break;
			case TILE_INDEX_COIN: 
				ret_val = SpriteCoin;	break;
			case TILE_INDEX_SPIRIT1: 
				ret_val = SpriteEnemy1;	break;
			case TILE_INDEX_SPIRIT2: 
				ret_val = SpriteSpirit;	break;
			case TILE_INDEX_BAT: 
				ret_val = SpriteBat;	break;
			case TILE_INDEX_TOADSTALL: 
				break;
			default: 
				break;
		}
		*tile = *(tile_ptr+1);
		return ret_val;
	}
	return 255u;
}*/

UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	if(current_state == StateGame) {
		if (U_LESS_THAN(255  - (UINT16)*tile_ptr, N_SPRITE_TYPES)) {
			*tile = *(tile_ptr+1);
			return 255 - (UINT16)*tile_ptr;
		}
		*tile = *tile_ptr;
	}
	return 255u;
}