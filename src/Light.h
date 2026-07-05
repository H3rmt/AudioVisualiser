#pragma once
#include "Core.hpp"
#include "MultiplexedStrip.hpp"

constexpr int STRIP1OUT = 12;
constexpr int STRIP2OUT = 13;

void setupLeds();

void selectOutput(const uint8_t index);

void renderStrip(
    MultiplexedStrip &strip,
    const uint8_t index,
    const LEDSettings &settings, const uint16_t level, const uint16_t offset,
    const float divider, const uint16_t width, const uint16_t bars,
    const float speed,
    const bool reverseOnPeak
);


void updateBrightness();

MultiplexedStrip *one();

MultiplexedStrip *two();
