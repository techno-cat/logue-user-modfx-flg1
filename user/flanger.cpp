/*
Copyright 2019 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include "usermodfx.h"
#include "buffer_ops.h"
#include "LCWDelay.h"
#include "LCWFixedMath.h"
#include "LCWLowFreqOsc.h"

static __sdram int32_t s_delay_ram_input[LCW_DELAY_INPUT_SIZE];
static __sdram int32_t s_delay_ram_sampling[LCW_DELAY_SAMPLING_SIZE];

static float s_inputGain;
static float s_time;
static float s_depth;

static LCWLowFreqOscBlock s_lfo;

#define FB_GAIN_TABLE_SIZE (9)
static const float fbGainTable[FB_GAIN_TABLE_SIZE] = {
  0.000000f,
  0.204645f,
  0.442431f,
  0.637438f,
  0.773636f,
  0.862647f,
  0.919714f,
  0.956597f,
  0.981079f
};

__fast_inline float lut_feedback(float x)
{
  if ( x < .5f ) {
    return 0.f;
  }

  // 0.5〜1.0 => 0.0〜(size-1)
  const float x2 = (x - .5f) * 2.f * (FB_GAIN_TABLE_SIZE - 1);
  const int32_t i = (int32_t)x2;
  if ( i < (FB_GAIN_TABLE_SIZE - 1) ) {
    const float val = fbGainTable[i];
    const float diff = fbGainTable[i+1] - val;
    return val + (diff * (x2 - i));
  }
  else {
    return fbGainTable[FB_GAIN_TABLE_SIZE - 1];
  }
}

// limit : equal or more than 1.f
__fast_inline float softlimiter(float c, float x, float limit)
{
  float th = limit - 1.f + c;
  float xf = si_fabsf(x);
  if ( xf < th ) {
    return x;
  }
  else {
    return si_copysignf( th + fx_softclipf(c, xf - th), x );
  }
}

void MODFX_INIT(uint32_t platform, uint32_t api)
{
  const LCWDelayNeededBuffer buffer = {
    s_delay_ram_input,
    s_delay_ram_sampling
  };

  LCWDelayInit( &buffer );
  LCWDelayReset();

  s_time = 0.25f;
  s_depth = 0.5f;
  s_inputGain = 0.f;

  s_lfo.dt = LCW_SQ7_24(1.0);
  s_lfo.th = LCW_SQ7_24(0.5);
  s_lfo.out = 0;
  s_lfo.out2 = 0;
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn,  float *sub_yn,
                   uint32_t frames)
{
  const float wet = 0.4999f;// s_depth;
  const float dry = 0.5f;// 1.f - wet;

  const float depth = clampmaxfsel( .5f + s_depth, 1.f );
  const float fb = lut_feedback( s_depth );

  const float * mx = main_xn;
  float * __restrict my = main_yn;
  const float * my_e = my + 2*frames;
  const float * sx = sub_xn;
  float * __restrict sy = sub_yn;

  int32_t time = q16_pow2( LCW_SQ15_16(-4.2f + ((1.f - s_time) * 8.2f)) );
  s_lfo.dt = LCW_LFO_TIMER_MAX / (uint32_t)(((time >> 4) * (48000 >> 6)) >> 6);
  int32_t lfoDepth = q16_pow2( LCW_SQ15_16(-3.0f + (depth * 2.9f)) ) >> 8;

  int32_t delaySamples = 120;

  for (; my != my_e; ) {
    float xL = *mx;
    float wL = LCWDelayOutput(delaySamples) / (float)(1 << 24);

    lfo_inc( &s_lfo );

    SQ7_24 lfoOut = (SQ7_24)( ((int64_t)(s_lfo.out2 - (s_lfo.th >> 1)) * LCW_SQ15_16(4.0)) >> 16 );
    LCWDelayUpdate( (((lfoOut >> 4) * lfoDepth) >> 4) - LCW_SQ7_24(1.0) );

    float fbL = wL * fb;
    LCWDelayInput( (int32_t)(((xL * s_inputGain) + fbL) * (1 << 24)) );

    wL = softlimiter( 0.1f, wL, 1.f );
    float yL = (dry * xL) + (wet * wL);

    mx += 2;
    sx += 2;
    *(my++) = yL;
    *(my++) = yL;
    *(sy++) = yL;
    *(sy++) = yL;

    if ( s_inputGain < 0.99998f ) {
      s_inputGain += ( (1.f - s_inputGain) * 0.0625f );
    }
    else { s_inputGain = 1.f; }
  }
}

void MODFX_RESUME(void)
{
  buf_clr_u32(
    (uint32_t * __restrict__)s_delay_ram_input,
    LCW_DELAY_INPUT_SIZE );
  buf_clr_u32(
    (uint32_t * __restrict__)s_delay_ram_sampling,
    LCW_DELAY_SAMPLING_SIZE );
  s_inputGain = 0.f;
}

void MODFX_PARAM(uint8_t index, int32_t value)
{
  const float valf = q31_to_f32(value);
  switch (index) {
  case k_user_modfx_param_time:
    s_time = clip01f(valf);
    break;
  case k_user_modfx_param_depth:
    s_depth = clip01f(valf);
    break;
  default:
    break;
  }
}
