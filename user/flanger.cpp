/*
Copyright 2019 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include "usermodfx.h"
#include "buffer_ops.h"
#include "LCWDelay.h"
#include "LCWFixedMath.h"
#include "LCWLowFreqOsc.h"
#include "LCWClipCurveTable.h"

static __sdram int32_t s_delay_ram_input[LCW_DELAY_INPUT_SIZE];
static __sdram int32_t s_delay_ram_sampling[LCW_DELAY_SAMPLING_SIZE];

static float s_inputGain;
static float s_time;
static float s_depth;

static LCWLowFreqOscBlock s_lfo;

#define LCW_FEEDBACK_GAIN (0.890230f)

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

__fast_inline float lut_clipcurvef(float x) {
  const float xf = si_fabsf(clampmaxfsel(x, 2.f)) * (1 << LCW_CLIP_CURVE_FRAC_BITS);
  const uint32_t xi = (uint32_t)x;
  const float y0 = gLcwClipCurveTable[xi];
  const float y1 = gLcwClipCurveTable[xi+1];
  return si_copysignf(linintf(xf - xi, y0, y1), x);
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
  
  s_lfo.dt = 0x10000;
  s_lfo.th = (int32_t)(0.5f * 0x10000);
  s_lfo.out = 0.f;
  s_lfo.out2 = 0.f;
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn,  float *sub_yn,
                   uint32_t frames)
{
  const float wet = fx_softclipf( 1/3.f, s_depth * 4.f );
  const float dry = 1.f - (wet * 0.5f);
  const float fb = LCW_FEEDBACK_GAIN;

  const float * mx = main_xn;
  float * __restrict my = main_yn;
  const float * my_e = my + 2*frames;
  const float * sx = sub_xn;
  float * __restrict sy = sub_yn;

  int32_t time = q16_pow2( LCW_SQ15_16(-4.2f + ((1.f - s_time) * 8.2f)) );
  s_lfo.dt = LCW_LFO_TIMER_MAX / (uint32_t)(((time >> 4) * (48000 >> 6)) >> 6);
  int32_t lfoDepth = q16_pow2( LCW_SQ15_16(-3.f + (s_depth * 5.f)) ) >> 8;

  for (; my != my_e; ) {
    float xL = *mx;
    float wL = LCWDelayOutput() / (float)(1 << 24);

    lfo_inc( &s_lfo );
    // s15.16 -> s7.24
    LCWDelayUpdate( ((s_lfo.th >> 1) - s_lfo.out2) * lfoDepth );

    float fbL = softlimiter(0.1f, wL, 1.2f) * fb;
    LCWDelayInput( (int32_t)(((xL * s_inputGain) + fbL) * (1 << 24)) );

    float yL = lut_clipcurvef( (dry * xL) + (wet * wL) );

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
