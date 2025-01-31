#include "Banks/SetAutoBank.h"
#include "main.h"

#include <gb/gb.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Keys.h"
#include "Music.h"

#include "GameTypes.h"

IMPORT_MAP(timeup);
DECLARE_MUSIC(timeup);


void START() {
	HIDE_HUD;
	InitScroll(BANK(timeup), &timeup, 0, 0);
	PlayMusic(timeup, 0);
}

void UPDATE() {
	if (ANY_KEY_PRESSED) {
		SetState(StateGame);
	}
}