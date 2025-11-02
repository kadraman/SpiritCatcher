#include <stdbool.h>
#include <gbdk/emu_debug.h>

UINT8   g_stage_current = 1;
UINT8   g_level_current = 1;
UINT8   g_player_lives = 0;
UINT8   g_next_portal = 1;      // Not used - remove?
UINT16	g_player_score = 0;