#pragma once
#include <array>
#include <utility>
#include <cstdint>
#include "globals.h"

struct ColorRGB{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
struct ColorHSL{
    int16_t hue;
    double saturation;
    double lighting;
};

ColorHSL rgbToHsl(const ColorRGB& color);
ColorRGB mapValueToGradient(int64_t val, int64_t low_end, int64_t high_end);    