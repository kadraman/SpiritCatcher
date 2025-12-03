#ifndef SPRITE_SPIRIT_H
#define SPRITE_SPIRIT_H

#include <gb/gb.h>

// Spirit animations and movement lookup table
const UINT8 anim_spirit[] = {3, 0, 1, 2};
const UINT8 anim_spirit_caught[] = {5, 3, 3, 3, 3, 3};
// sine-like movement table used to move the spirit in a circular pattern
// (size inferred from initializer to avoid mismatches)
const INT8 sin_table[] = {
      0,   4,   6,  10,  12,  16,  18,  22,
     24,  28,  30,  34,  36,  40,  42,  46,
     47,  51,  53,  57,  58,  62,  63,  67,
     68,  72,  73,  77,  77,  81,  81,  85,
     85,  89,  89,  92,  92,  95,  95,  98,
     98, 101, 101, 103, 103, 105, 105, 107,
    107, 108, 108, 109, 109, 110, 110, 111,
    111, 111, 111, 111, 111, 111, 111, 111,
    111, 111, 111, 111, 111, 111, 111, 111,
    110, 110, 109, 109, 108, 108, 107, 107,
    105, 105, 103, 103, 101, 101,  98,  98,
     95,  95,  92,  92,  89,  89,  85,  85,
     81,  81,  77,  77,  73,  72,  68,  67,
     63,  62,  58,  57,  53,  51,  47,  46,
     42,  40,  36,  34,  30,  28,  24,  22,
     18,  16,  12,  10,   6,   4,   0,  -4,
    -6, -10, -12, -16, -18, -22, -24, -28,
   -30, -34, -36, -40, -42, -46, -47, -51,
   -53, -57, -58, -62, -63, -67, -68, -72,
   -73, -77, -77, -81, -81, -85, -85, -89
};

#endif // SPRITE_SPIRIT_H
