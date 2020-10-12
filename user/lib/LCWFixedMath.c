/*
Copyright 2019 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include "LCWFixedMath.h"

#define LCW_POW2_TABLE_BITS (6)
#define LCW_POW2_TABLE_SIZE (1 << LCW_POW2_TABLE_BITS)
#define LCW_POW2_TABLE_MASK (LCW_POW2_TABLE_SIZE - 1)
static const UQ8_24 pow2Table[LCW_POW2_TABLE_SIZE + 1] = {
    0x01000000, 0x0102C9A3, 0x01059B0D, 0x01087451, 0x010B5586, 0x010E3EC3, 0x0111301D, 0x011429AA,
    0x01172B83, 0x011A35BE, 0x011D4873, 0x012063B8, 0x012387A6, 0x0126B456, 0x0129E9DF, 0x012D285A,
    0x01306FE0, 0x0133C08B, 0x01371A73, 0x013A7DB3, 0x013DEA64, 0x014160A2, 0x0144E086, 0x01486A2B,
    0x014BFDAD, 0x014F9B27, 0x015342B5, 0x0156F473, 0x015AB07D, 0x015E76F1, 0x016247EB, 0x01662388,
    0x016A09E6, 0x016DFB23, 0x0171F75E, 0x0175FEB5, 0x017A1147, 0x017E2F33, 0x01825899, 0x01868D99,
    0x018ACE54, 0x018F1AE9, 0x0193737B, 0x0197D829, 0x019C4918, 0x01A0C667, 0x01A5503B, 0x01A9E6B5,
    0x01AE89F9, 0x01B33A2B, 0x01B7F76F, 0x01BCC1E9, 0x01C199BD, 0x01C67F12, 0x01CB720D, 0x01D072D4,
    0x01D5818D, 0x01DA9E60, 0x01DFC973, 0x01E502EE, 0x01EA4AFA, 0x01EFA1BE, 0x01F50765, 0x01FA7C18,
    0x02000000
};

SQ15_16 q16_pow2(SQ15_16 x)
{
    const int32_t lshift = x >> 16;
    const uint32_t frac = (uint32_t)(x & 0xFFFF);
    uint32_t i = frac >> (16 - LCW_POW2_TABLE_BITS);
    uint32_t tmp = frac & (0xFFFF >> LCW_POW2_TABLE_BITS);

    // q24 -> q16
    SQ15_16 val = (SQ15_16)pow2Table[i] >> (24 - 16);
    SQ15_16 diff = ((SQ15_16)pow2Table[i+1] >> (24 - 16)) - val;
    val += ( (tmp * diff) >> (16 - LCW_POW2_TABLE_BITS) );

    if ( lshift < 0 ) {
        return val >> -lshift;
    }
    else {
        return val << lshift;
    }
}

SQ7_24 q24_pow2(SQ7_24 x)
{
    const int32_t lshift = x >> 24;
    const uint32_t frac = (uint32_t)(x & 0xFFFFFF);
    uint32_t i = frac >> (24 - LCW_POW2_TABLE_BITS);
    uint32_t tmp = frac & (0xFFFFFF >> LCW_POW2_TABLE_BITS);

    SQ7_24 val = (SQ7_24)pow2Table[i];
    SQ7_24 diff = (SQ7_24)pow2Table[i+1] - val;
    val += (SQ7_24)( (int64_t)(tmp * diff) >> (24 - LCW_POW2_TABLE_BITS) );

    if ( lshift < 0 ) {
        return val >> -lshift;
    }
    else {
        return val << lshift;
    }
}
