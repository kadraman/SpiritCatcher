#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define STATES \
_STATE(StateSplash)\
_STATE(StateGame)\
_STATE(StateTimeUp)\
_STATE(StateGameOver)\
_STATE(StateWin)\
STATE_DEF_END

#define SPRITES \
_SPRITE(SpriteMushroom, mushroom,   FLIP_NONE)\
_SPRITE(SpriteBat,      bat,        FLIP_NONE)\
_SPRITE(SpriteSpirit,   spirit,     FLIP_NONE)\
_SPRITE(SpriteEnemy1,   enemy1,     FLIP_NONE)\
_SPRITE(SpriteCoin,     coin,       FLIP_NONE)\
_SPRITE(SpritePortal,   portal,     FLIP_NONE)\
_SPRITE(SpritePlayer,   player,     FLIP_NONE)\
_SPRITE(SpriteKnife,    knife,      FLIP_NONE)\
_SPRITE(SpriteBullet,   bullet,     FLIP_NONE)\
_SPRITE(SpriteParticle, particles,  FLIP_NONE)\
_SPRITE(SpriteJewell1,  jewell1,    FLIP_NONE)\
_SPRITE(SpriteAmmo,     ammo,       FLIP_NONE)\
_SPRITE(SpriteFlag,     flag,       FLIP_NONE)\
_SPRITE(SpriteAttack1,  attack1,    FLIP_NONE)\
SPRITE_DEF_END

#include "ZGBMain_Init.h"

#endif // ZGBMAIN_H