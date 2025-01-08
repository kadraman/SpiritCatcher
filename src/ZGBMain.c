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
	if(current_state == StateGame) {
		if (U_LESS_THAN(255  - (UINT16)*tile_ptr, N_SPRITE_TYPES)) {
			*tile = *(tile_ptr+1);
			return 255 - (UINT16)*tile_ptr;
		}
		*tile = *tile_ptr;
	}
	return 255u;
}