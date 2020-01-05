/*
Copyright 2019 Tomoaki Itoh
This software is released under the MIT License, see LICENSE.txt.
//*/

#include "LCWClipCurveTable.h"

// f(t,a) = tanh(abs(t)^a) ** (1/a), a = exp(1.414)
const float gLcwClipCurveTable[LCW_CLIP_CURVE_TABLE_SIZE] = {
    0.000000, 0.015625, 0.031250, 0.046875,
    0.062500, 0.078125, 0.093750, 0.109375,
    0.125000, 0.140625, 0.156250, 0.171875,
    0.187500, 0.203125, 0.218750, 0.234375,
    0.250000, 0.265625, 0.281249, 0.296874,
    0.312498, 0.328122, 0.343746, 0.359369,
    0.374990, 0.390611, 0.406230, 0.421847,
    0.437460, 0.453070, 0.468675, 0.484274,
    0.499865, 0.515445, 0.531013, 0.546566,
    0.562099, 0.577610, 0.593091, 0.608539,
    0.623945, 0.639302, 0.654600, 0.669828,
    0.684975, 0.700026, 0.714966, 0.729776,
    0.744437, 0.758928, 0.773223, 0.787298,
    0.801123, 0.814669, 0.827904, 0.840795,
    0.853307, 0.865407, 0.877061, 0.888235,
    0.898898, 0.909021, 0.918579, 0.927551,
    0.935920, 0.943677, 0.950816, 0.957340,
    0.963258, 0.968583, 0.973337, 0.977546,
    0.981241, 0.984456, 0.987228, 0.989596,
    0.991601, 0.993280, 0.994674, 0.995819,
    0.996751, 0.997500, 0.998096, 0.998565,
    0.998931, 0.999212, 0.999426, 0.999586,
    0.999706, 0.999793, 0.999856, 0.999902,
    0.999933, 0.999956, 0.999971, 0.999981,
    0.999988, 0.999992, 0.999995, 0.999997,
    0.999998, 0.999999, 0.999999, 1.000000,
    1.000000, 1.000000, 1.000000, 1.000000,
    1.000000, 1.000000, 1.000000, 1.000000,
    1.000000, 1.000000, 1.000000, 1.000000,
    1.000000, 1.000000, 1.000000, 1.000000,
    1.000000, 1.000000, 1.000000, 1.000000,
    1.000000, 1.000000, 1.000000, 1.000000,
    1.000000
};
