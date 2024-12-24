#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Keys.h"
#include "Music.h"

#include "GlobalVars.h"

IMPORT_MAP(titlescreen);
DECLARE_MUSIC(organintro);

void START() {
	InitScroll(BANK(titlescreen), &titlescreen, 0, 0);
	PlayMusic(organintro, 0);
}

void UPDATE() {
	if (ANY_KEY_PRESSED) {
		g_level_current = 1;
		SetState(StateGame);
	}
}