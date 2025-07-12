#include "Banks/SetAutoBank.h"
#include "main.h"
#include <gbdk/platform.h>
#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "TilesInfo.h"
#include "MapInfo.h"

#include <gb/gb.h>
#include "ZGBMain.h"
#include "Scroll.h"
#include "Keys.h"
#include "Music.h"

#include "GameTypes.h"

IMPORT_MAP(titlescreen);
DECLARE_MUSIC(organintro);

// Define your palettes
static const palette_color_t palette1[] = { RGB8(255,255,255), RGB8(237,237,211), RGB8(114,114,114), RGB8(  0,  0,  0) };
static const palette_color_t palette2[] = { RGB(0,0,0), RGB(0,0,0), RGB(0,0,0), RGB(0,0,0) };

UINT8 palette_timer = 0;
UINT8 palette_state = 0;

void START() {
	InitScroll(BANK(titlescreen), &titlescreen, 0, 0);
	//set_bkg_palette(BKGF_CGB_PAL0, 3, palette2);
	//PlayMusic(organintro, 0);
	palette_timer = 0;
	palette_state = 0;
}

void UPDATE() {
	/*palette_timer++;
	if (palette_timer >= 60) { // Change every 60 frames (~1 second at 60Hz)
		palette_timer = 0;
		palette_state ^= 1;
		if (palette_state) {
			set_bkg_palette(BKGF_CGB_PAL0, 0, palette2);
		} else {
			set_bkg_palette(BKGF_CGB_PAL0, 0, palette1);
		}
	}*/

	if (ANY_KEY_PRESSED) {
		g_level_current = 1;
		SetState(StateGame);
	}
}