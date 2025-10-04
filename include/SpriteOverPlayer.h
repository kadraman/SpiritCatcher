#ifndef SPITREOVERPLAYER_H
#define SPITREOVERPLAYER_H

#include <stdbool.h>
#include "main.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "ZGBMain.h"
#include "ZGBUtils.h"

#define DEFAULT_ANIM_SPEED      8

typedef struct {
    UINT8 start_x;
    UINT8 start_y;
    UINT8 lives;    // no longer used
} OverPlayerData;
ASSERT_CUSTOM_DATA_SIZE(OverPlayerData, DEFAULT_CUSTOM_DATA_SIZE);

#endif // SPITREOVERPLAYER_H