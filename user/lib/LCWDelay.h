/*
Copyright 2019 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#ifndef LCWDelay_h
#define LCWDelay_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define LCW_DELAY_INPUT_BITS (5)
#define LCW_DELAY_INPUT_SIZE (1 << LCW_DELAY_INPUT_BITS)
#define LCW_DELAY_INPUT_MASK (LCW_DELAY_INPUT_SIZE - 1)

#define LCW_DELAY_SAMPLING_BITS (14)
#define LCW_DELAY_SAMPLING_SIZE (1 << LCW_DELAY_SAMPLING_BITS)
#define LCW_DELAY_SAMPLING_MASK (LCW_DELAY_SAMPLING_SIZE - 1)

typedef struct {
    int32_t *input;
    int32_t *sampling;
} LCWDelayNeededBuffer;

extern void LCWDelayInit(const LCWDelayNeededBuffer *buffer);
extern void LCWDelayReset(void);
extern void LCWDelayUpdate(int32_t delayTime);
extern void LCWDelayInput(int32_t fxSend);
extern int32_t LCWDelayOutput(int32_t delaySamples);

#ifdef __cplusplus
}
#endif

#endif /* LCWDelay_h */
