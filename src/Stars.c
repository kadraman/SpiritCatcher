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

#define STARS_TILE_1 1
#define STARS_TILE_2 2
#define STARS_TILE_3 3
#define STARS_TILE_4 4
#define STARS_TILE_5 5
#define STARS_TILE_6 6
#define STARS_TILE_7 7
#define STARS_TILE_8 8
#define STARS_TILE_9 9
#define STARS_TILE_10 10
#define STARS_TILE_11 11

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