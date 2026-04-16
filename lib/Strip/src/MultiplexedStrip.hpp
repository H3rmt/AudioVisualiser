#pragma once

#include <Adafruit_NeoPixel.h>

#include "Animations.hpp"


class MultiplexedStrip {
public:
    MultiplexedStrip(int16_t pin, uint16_t selectA, uint16_t selectB, uint16_t ledCount1, uint16_t ledCount2,
                     uint16_t ledCount3, uint16_t ledCount4);

    void begin();

    void setReversed(uint8_t index, bool reverse);

    void test(uint8_t index);

    void offAnimation(uint8_t index);

    void normal(uint8_t index, uint32_t lvl);

    void centre(uint8_t index, uint32_t lvl);

    void setMaxBrightness(long brightness);

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

    static void updateColorOffset(Animations::StripData *current);
};
