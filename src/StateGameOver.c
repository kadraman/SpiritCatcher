#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Keys.h"
#include "Music.h"

#include "GameTypes.h"

IMPORT_MAP(gameover);
DECLARE_MUSIC(gameover);

void START() {
	HIDE_HUD;
	InitScroll(BANK(gameover), &gameover, 0, 0);
	PlayMusic(gameover, 0);
}

void UPDATE() {
	if (ANY_KEY_PRESSED) {
		// Reset the game
        g_game_reset = true;
        g_player_lives = MAX_LIVES;
		g_stage_current = 1;
        g_level_current = 1;
		SetState(StateOverworld);
	}
}