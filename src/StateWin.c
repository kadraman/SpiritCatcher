#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Keys.h"
#include "Music.h"

#include "GlobalVars.h"

IMPORT_MAP(win);
DECLARE_MUSIC(victoryloop);

void START() {
	HIDE_HUD;
	InitScroll(BANK(win), &win, 0, 0);
	PlayMusic(victoryloop, 1);
}

void UPDATE() {
	if(KEY_TICKED(J_START) | KEY_TICKED(J_A) | KEY_TICKED(J_B)) {
		g_level_current = 1;
		SetState(StateGame);
	}
}