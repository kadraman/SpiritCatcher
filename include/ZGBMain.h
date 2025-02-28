#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define STATES \
_STATE(StateSplash)\
_STATE(StateOverworld)\
_STATE(StateGame)\
_STATE(StateTimeUp)\
_STATE(StateGameOver)\
_STATE(StateWin)\
STATE_DEF_END

#define SPRITES \
_SPRITE(SpritePlatform, platform,   FLIP_NONE)\
_SPRITE(SpriteMushroom, mushroom,   FLIP_NONE)\
_SPRITE(SpriteBat,      bat,        FLIP_NONE)\
_SPRITE(SpriteSlime,    slime,      FLIP_NONE)\
_SPRITE(SpriteSpirit,   spirit,     FLIP_NONE)\
_SPRITE(SpriteHealth,   health,     FLIP_NONE)\
_SPRITE(SpriteMana,     mana,       FLIP_NONE)\
_SPRITE(SpritePortal,   portal,     FLIP_NONE)\
_SPRITE(SpritePlayer,   player,     FLIP_NONE)\
_SPRITE(SpriteMagix,    magix,      FLIP_NONE)\
_SPRITE(SpriteParticle, particles,  FLIP_NONE)\
_SPRITE(SpriteAttack1,  attack1,    FLIP_NONE)\
SPRITE_DEF_END

#include "ZGBMain_Init.h"

#endif // ZGBMAIN_H