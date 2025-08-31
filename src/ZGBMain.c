#include "ZGBMain.h"
#include "Math.h"
#include "StateGame.h"
#include "GameTypes.h"
#include <gbdk/emu_debug.h>

#ifdef NDEBUG
UINT8 next_state = StateSplash;
#else
UINT8 next_state = StateOverworld;
#endif

UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	if (current_state == StateOverworld || StateGame) {
		if (U_LESS_THAN(255 - (UINT16)*tile_ptr, N_SPRITE_TYPES)) {
			// NOTE: for some reason tile to the right of current tile (tile_ptr+1) will be the replaced tile!
			*tile = *(tile_ptr+1);
			if (current_state == StateOverworld) {
				return 255 - (UINT16)*tile_ptr + SPRITE_OVERWORLD_OFFSET; // overworld sprites start at tile 14
			} else {
				return 255 - (UINT16)*tile_ptr; // game sprites start at tile 0
			}
		}
		*tile = *tile_ptr;
	}
	return 255u;
}