#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Keys.h"
#include "Music.h"

#include "GlobalVars.h"

IMPORT_MAP(gameover);
DECLARE_MUSIC(gameover);

void START() {
	HIDE_HUD;
	InitScroll(BANK(gameover), &gameover, 0, 0);
	PlayMusic(gameover, 0);
}

void UPDATE() {
	if (ANY_KEY_PRESSED) {
		SetState(StateGame);
	}
}