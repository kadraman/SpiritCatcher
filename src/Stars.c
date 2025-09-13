#include "Banks/SetAutoBank.h"

#include <gbdk/platform.h>
#include "ZGBMain.h"
#include "Music.h"
#include "SpriteManager.h"
#include "Palette.h"

static const unsigned char level_stars1[] = {
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff
};

static const unsigned char level_stars2[] = {
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xef,0xef,
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff
};

static const unsigned char level_stars3[] = {
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xef,
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff
};

static const unsigned char level_stars4[] = {
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xef,0xff,
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff
};

static const unsigned char level_stars5[] = {
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xfb,0xfb,
	0xff,0xff,0xff,0xff
};

static const unsigned char level_stars6[] = {
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xfb,
	0xff,0xff,0xff,0xff
};

static const unsigned char level_stars7[] = {
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,
	0xff,0xff,0xfb,0xff,
	0xff,0xff,0xff,0xff
};

#define STARS_TILE_BASE 27
#define STARS_TILE_1 (STARS_TILE_BASE + 0)
#define STARS_TILE_2 (STARS_TILE_BASE + 1)
#define STARS_TILE_3 (STARS_TILE_BASE + 2)
#define STARS_TILE_4 (STARS_TILE_BASE + 3)
#define STARS_TILE_5 (STARS_TILE_BASE + 4)
#define STARS_TILE_6 (STARS_TILE_BASE + 5)
#define STARS_TILE_7 (STARS_TILE_BASE + 6)
#define STARS_TILE_8 (STARS_TILE_BASE + 7)
#define STARS_TILE_9 (STARS_TILE_BASE + 8)
#define STARS_TILE_10 (STARS_TILE_BASE + 9)
#define STARS_TILE_11 (STARS_TILE_BASE + 10)

UINT8 anim_stars_counter = 0;

void Stars_Animate(void) BANKED {
	if (anim_stars_counter == 0) {
		set_bkg_data (STARS_TILE_2, 1, level_stars2);
		set_bkg_data (STARS_TILE_3, 1, level_stars3);
		set_bkg_data (STARS_TILE_4, 1, level_stars4);

		set_bkg_data (STARS_TILE_5, 1, level_stars7);
		set_bkg_data (STARS_TILE_6, 1, level_stars5);
		set_bkg_data (STARS_TILE_7, 1, level_stars6);
	} else if (anim_stars_counter == 20) {
		set_bkg_data(STARS_TILE_2, 1, level_stars3);
		set_bkg_data(STARS_TILE_3, 1, level_stars4);
		set_bkg_data(STARS_TILE_4, 1, level_stars2);

		set_bkg_data (STARS_TILE_5, 1, level_stars5);
		set_bkg_data (STARS_TILE_6, 1, level_stars6);
		set_bkg_data (STARS_TILE_7, 1, level_stars7);
	} else if (anim_stars_counter == 40) {
		set_bkg_data(STARS_TILE_2, 1, level_stars4);
		set_bkg_data(STARS_TILE_3, 1, level_stars2);
		set_bkg_data(STARS_TILE_4, 1, level_stars3);
		
		set_bkg_data (STARS_TILE_5, 1, level_stars6);
		set_bkg_data (STARS_TILE_6, 1, level_stars7);
		set_bkg_data (STARS_TILE_7, 1, level_stars5);
	}
	anim_stars_counter++;
	if (anim_stars_counter >= 60) anim_stars_counter = 0;
}