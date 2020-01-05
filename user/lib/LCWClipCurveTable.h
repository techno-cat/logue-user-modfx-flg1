/*
Copyright 2019 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include <stdint.h>

#define LCW_CLIP_CURVE_FRAC_BITS (6)
#define LCW_CLIP_CURVE_TABLE_SIZE (129)

#define LCW_CLIP_CURVE_VALUE_BITS (24)
#define LCW_CLIP_CURVE_VALUE_MAX (1 << (LCW_CLIP_CURVE_VALUE_BITS))

extern const float gLcwClipCurveTable[LCW_CLIP_CURVE_TABLE_SIZE];

