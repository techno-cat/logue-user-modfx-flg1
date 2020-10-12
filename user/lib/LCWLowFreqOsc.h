/*
Copyright 2019 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#ifndef LCWLowFreqOsc_h
#define LCWLowFreqOsc_h

#include <LCWCommon.h>

#define LCW_LFO_TIMER_BITS (30)
#define LCW_LFO_TIMER_MAX (1 << LCW_LFO_TIMER_BITS)
#define LCW_LFO_TIMER_MASK (LCW_LFO_TIMER_MAX - 1)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint32_t dt;
  uint32_t t;
  uint32_t dir; // 0: +, 1: -
  int32_t th;   // s7.24
  int32_t out;  // s7.24
  int32_t out2; // s7.24
} LCWLowFreqOscBlock;

extern void lfo_inc(LCWLowFreqOscBlock *block);

#ifdef __cplusplus
}
#endif

#endif /* LCWLowFreqOsc_h */
