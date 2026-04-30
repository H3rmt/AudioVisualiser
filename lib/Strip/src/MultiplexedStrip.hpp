#pragma once

#include <Adafruit_NeoPixel.h>

#include "Animations.hpp"


class MultiplexedStrip {
public:
    MultiplexedStrip(int16_t pin, uint16_t selectA, uint16_t selectB, uint16_t ledCount1, uint16_t ledCount2,
                     uint16_t ledCount3, uint16_t ledCount4);

    void begin();

    void setReversed(uint8_t index, bool reverse);

    void setPerLedColorChange(uint8_t index, uint16_t change);

    void test(uint8_t index);

    void resetOff(uint8_t index);

    void offAnimation(uint8_t index);

    void normal(uint8_t index, uint16_t lvl, uint16_t colorOffset, float percentMaxChangeDivider = 20);

    void centre(uint8_t index, uint16_t lvl, uint16_t colorOffset, float percentMaxChangeDivider = 20);

    void circle(uint8_t index, uint16_t lvl, uint16_t colorOffset, uint16_t width = 5, uint16_t bars = 2,
                float moveSpeed = 0.1f, bool reverseOnPeak = false);

    void off(uint8_t index);

    void setMaxBrightness(uint8_t index, uint8_t brightness);

    void setMaxHWBrightness(uint8_t index, uint8_t brightness);

    void setRainbow(uint8_t index, bool rainbow);

private:
    Adafruit_NeoPixel pixels;

    uint16_t maxLength;
    Animations::StripData first;
    Animations::StripData second;
    Animations::StripData third;
    Animations::StripData fourth;

    uint16_t selectA;
    uint16_t selectB;

    Animations::StripData *selectStrip(uint8_t index);
};
