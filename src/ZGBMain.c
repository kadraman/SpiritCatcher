#include "ZGBMain.h"
#include "Math.h"
#include "StateGame.h"
#include "GameTypes.h"
#include <gbdk/emu_debug.h>

UINT16 g_player_score = 0;

#ifdef NDEBUG
UINT8 next_state = StateSplash;
#else
UINT8 next_state = StateGame;
#endif

UINT8 GetTileReplacement(UINT8* tile_ptr, UINT8* tile) {
	if (current_state == StateGame) {
		if (*tile_ptr > (255u - N_SPRITE_TYPES)) {
			EMU_printf("Replacing tile: %d:%d with %d\n", *tile_ptr, *tile, 255 - (UINT16)*tile_ptr);
			*tile = (*(tile_ptr + 1));
			EMU_printf("Replacing tile: %d:%d with %d\n", *tile_ptr, *tile, 255 - (UINT16)*tile_ptr);

			return 255 - *tile_ptr;
		} else {
			*tile = *tile_ptr;
			return 255u;
		}
		//if (U_LESS_THAN(255  - (UINT16)*tile_ptr, N_SPRITE_TYPES)) {
		//	*tile = *tile_ptr;
		//	EMU_printf("Replacing tile: %d %d with %d\n", *tile_ptr, *tile, 255 - (UINT16)*tile_ptr);
		//	return 255 - (UINT16)*tile_ptr;
		//}
		//*tile = *tile_ptr;*/
		//return (*tile_ptr > (255u - N_SPRITE_TYPES)) ? ((*tile = 0), (255u - *tile_ptr)) : ((*tile = *tile_ptr), 255);
	}
	return 255u;
}