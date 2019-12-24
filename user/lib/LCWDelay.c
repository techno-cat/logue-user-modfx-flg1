/*
Copyright 2019 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include "LCWDelay.h"
#include "LCWFixedMath.h"
#include "LCWDelayFirParamTable.h"

#define LCW_DELAY_BUFFER_DEC(buf) (((buf).pointer - 1) & (buf).mask)

// 入力バッファにおけるサンプリング位置（固定）
#define LCW_DELAY_SAMPLING_POS (1 + (LCW_DELAY_FIR_TAP >> 1))

typedef struct {
    SQ7_24 *buffer;
    uint32_t size;
    uint32_t mask;
    int32_t pointer;
} LCWDelayBuffer;

typedef struct {
    SQ15_16 current;
    SQ15_16 sampling;
    SQ7_24 step;
    uint32_t delayOffset; // u24.8
} LCWDelayBlock;

static LCWDelayBuffer delayInputBuffer = {
    (SQ7_24 *)0,
    LCW_DELAY_INPUT_SIZE,
    LCW_DELAY_INPUT_MASK,
    0
};
static LCWDelayBuffer delaySamplingBuffer = {
    (SQ7_24 *)0,
    LCW_DELAY_SAMPLING_SIZE,
    LCW_DELAY_SAMPLING_MASK,
    0
};

static LCWDelayBlock delayBlock;

static SQ7_24 resampling(const LCWDelayBuffer *p, int32_t i, const SQ3_12 *fir, int32_t n)
{
    const uint32_t mask = p->mask;
#if (1)
    int64_t ret = 0;
    for (int32_t j=0; j<n; j++) {
        ret += ( (int64_t)(p->buffer[(i + j) & mask]) * fir[j] );
    }

    return (SQ7_24)( ret >> LCW_DELAY_FIR_VALUE_BITS );
#else
    return p->buffer[(i + (n >> 1)) & mask];
#endif
}

void LCWDelayInit(const LCWDelayNeededBuffer *buffer)
{
    delayInputBuffer.buffer = (SQ7_24 *)buffer->input;
    delaySamplingBuffer.buffer = (SQ7_24 *)buffer->sampling;
}

void LCWDelayReset(void)
{
    // memo: 引数で与える？
    const uint32_t delaySize = 60;
/*
    delayInputBuffer.pointer = 0;
    for (int32_t i=0; i<LCW_DELAY_INPUT_SIZE; i++) {
        delayInputBuffer.buffer[i] = 0;
    }

    delaySamplingBuffer.pointer = 0;
    for (int32_t i=0; i<LCW_DELAY_SAMPLING_SIZE; i++) {
        delaySamplingBuffer.buffer[i] = 0;
    }
*/
    delayBlock.current = LCW_SQ15_16( LCW_DELAY_SAMPLING_POS );
    delayBlock.sampling = LCW_SQ15_16( LCW_DELAY_SAMPLING_POS );
    delayBlock.step = LCW_SQ7_24( 1.0 );
    delayBlock.delayOffset = delaySize << 8;
}

void LCWDelayUpdate(int32_t delayPitch)
{
    // SQ7.24 -> SQ15.16
    const SQ15_16 pitch = delayPitch >> 8;
    // SQ15.16 -> SQ7.24
    delayBlock.step = q16_pow2(pitch) << 8;
}

void LCWDelayInput(int32_t fxSend)
{
    delayInputBuffer.pointer = LCW_DELAY_BUFFER_DEC(delayInputBuffer);
    delayInputBuffer.buffer[delayInputBuffer.pointer] = (SQ7_24)fxSend;
    delayBlock.current += LCW_SQ15_16( 1.0 );

    const SQ15_16 step = (SQ15_16)( delayBlock.step >> 8 );
    while ( delayBlock.sampling <= delayBlock.current ) {
#if(1)
        const int32_t i = delayInputBuffer.pointer - (LCW_DELAY_FIR_TAP >> 1) + (int32_t)(delayBlock.current >> 16);
        const SQ3_12 *fir = gLcwDelayFirTable[ (delayBlock.current >> (16 - LCW_DELAY_FIR_TABLE_BITS)) & LCW_DELAY_FIR_TABLE_MASK ];
        const SQ7_24 sample = resampling( &delayInputBuffer, i, fir, LCW_DELAY_FIR_TAP );
#else
        const int32_t i = delayInputBuffer.pointer + (int32_t)(delayBlock.current >> 16);
        const SQ7_24 sample = delayInputBuffer.buffer[i & delayInputBuffer.mask];
#endif

        delaySamplingBuffer.pointer = LCW_DELAY_BUFFER_DEC(delaySamplingBuffer);
        delaySamplingBuffer.buffer[delaySamplingBuffer.pointer] = sample;
        delayBlock.current -= step;
    }
}

int32_t LCWDelayOutput(void)
{
    // memo:
    // currentがsamplingを超えないことを想定している
    const uint32_t tmp = (uint32_t)(delayBlock.sampling - delayBlock.current);

    // u16.16 -> u24.8
    uint32_t offset = (tmp << 8) / (delayBlock.step >> (24 - 16));
    offset += delayBlock.delayOffset;

    const LCWDelayBuffer *p = &delaySamplingBuffer;
    const int32_t i = p->pointer - (LCW_DELAY_FIR_TAP >> 1) + (int32_t)(offset >> 8);
    const SQ3_12 *fir = gLcwDelayFirTable[ (offset >> (8 - LCW_DELAY_FIR_TABLE_BITS)) & LCW_DELAY_FIR_TABLE_MASK ];

    return resampling( p, i, fir, LCW_DELAY_FIR_TAP );
}
