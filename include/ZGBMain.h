#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define ASSERT_CUSTOM_DATA_SIZE(type, size) _Static_assert(sizeof(type) <= (size), #type " exceeds " #size " bytes")

#define STATES \
_STATE(StateSplash)\
_STATE(StateOverworld)\
_STATE(StateGame)\
_STATE(StateTimeUp)\
_STATE(StateGameOver)\
_STATE(StateWin)\
STATE_DEF_END

#define SPRITE_OVERWORLD_OFFSET 14
#define SPRITES \
_SPRITE(SpritePlatform,     platform,   FLIP_NONE)\
_SPRITE(SpriteRockard,      rockard,    FLIP_NONE)\
_SPRITE(SpriteBat,          bat,        FLIP_NONE)\
_SPRITE(SpriteSlime,        slime,      FLIP_NONE)\
_SPRITE(SpriteSpirit,       spirit,     FLIP_NONE)\
_SPRITE(SpriteHealth,       health,     FLIP_NONE)\
_SPRITE(SpriteMana,         mana,       FLIP_NONE)\
_SPRITE(SpritePortal,       portal,     FLIP_NONE)\
_SPRITE(SpritePlayer,       player,     FLIP_NONE)\
_SPRITE(SpriteMagix,        magix,      FLIP_NONE)\
_SPRITE(SpriteParticle,     particles,  FLIP_NONE)\
_SPRITE(SpriteAttack1,      attack1,    FLIP_NONE)\
_SPRITE(SpriteLantern,      lantern,    FLIP_NONE)\
_SPRITE(SpriteRocklette,    rocklette,  FLIP_NONE)\
_SPRITE(SpriteOverPortal,   overportal, FLIP_NONE)\
_SPRITE(SpriteOverPlayer,   overplayer, FLIP_NONE)\
_SPRITE(SpriteOverPlayerEyes,   overplayereyes, FLIP_NONE)\
SPRITE_DEF_END

#include "ZGBMain_Init.h"

#endif // ZGBMAIN_H