#include "Banks/SetAutoBank.h"

#include <gbdk/platform.h>
#include "ZGBMain.h"
#include "Music.h"
#include "SpriteManager.h"
#include "Palette.h"

static const unsigned char level_bridge1[] = {
	0x08,0x00,0x34,0x08,
	0xc3,0x3c,0x00,0xfb,
	0x00,0xdf,0x08,0xf7,
	0x80,0x7b,0x02,0xfd
};

static const unsigned char level_bridge2[] = {
    0x20,0x00,0xd3,0x20,
	0x0c,0xf3,0x00,0xff,
	0x20,0xdf,0x00,0xf7,
	0x80,0x7f,0x02,0xfd
};

static const unsigned char level_bridge3[] = {
	0x82,0x00,0x4d,0x82,
	0x30,0xcf,0x00,0xff,
	0x20,0xdf,0x00,0xff,
	0x80,0x7f,0x02,0xfd
};

#define BRIDGE_TILE_1 (0x1D) // 29
#define BRIDGE_TILE_2 (0x1E) // 30
#define BRIDGE_TILE_3 (0x1F) // 31

UINT8 anim_bridge_counter = 0;

void Bridge_Animate(void) BANKED {
	if (anim_bridge_counter == 0) {
		set_bkg_data (BRIDGE_TILE_1, 1, level_bridge1);
		set_bkg_data (BRIDGE_TILE_2, 1, level_bridge2);
		set_bkg_data (BRIDGE_TILE_3, 1, level_bridge3);
	} else if (anim_bridge_counter == 10) {
		set_bkg_data(BRIDGE_TILE_1, 1, level_bridge2);
		set_bkg_data(BRIDGE_TILE_2, 1, level_bridge3);
		set_bkg_data(BRIDGE_TILE_3, 1, level_bridge1);
	} else if (anim_bridge_counter == 20) {
		set_bkg_data(BRIDGE_TILE_1, 1, level_bridge3);
		set_bkg_data(BRIDGE_TILE_2, 1, level_bridge1);
		set_bkg_data(BRIDGE_TILE_3, 1, level_bridge2);
	}
	anim_bridge_counter++;
	if (anim_bridge_counter >= 30) anim_bridge_counter = 0;
}