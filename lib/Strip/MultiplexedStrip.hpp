#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

struct CircleAnimState {
    float movement = 0.0; // movement speed
    uint32_t last_dir_change = 0; // last direction change
    uint32_t circlePosition = 0; // current position in the circle
};

struct StripData {
    // set once
    uint16_t ledCount;
    bool reversed;
    bool rainbow;
    bool adaptiveBrightness;
    uint16_t colorChangeSpeed;
    uint8_t maxBrightness;

    // change
    uint32_t colorOffset = 0;
    CircleAnimState circleAnimState;
};

class MultiplexedStrip {
public:
    MultiplexedStrip(int16_t pin, uint16_t selectA, uint16_t selectB, uint16_t ledCount1, uint16_t ledCount2,
                     uint16_t ledCount3, uint16_t ledCount4);

    void begin();

    void setReversed(uint8_t index, bool reverse);

    // void setRainbow(uint8_t index, bool rainbow);
    // void setAdaptiveBrightness(uint8_t index, bool adaptive);
    // void setColorChangeSpeed(uint8_t index, uint16_t speed);
    // void setMaxBrightness(uint8_t index, uint8_t maxBrightness);

    void test(uint8_t index);

    // void test2(uint8_t index);
    // void test3(uint8_t index);
    // void test4(uint8_t index);
    // void off(uint8_t index);
    // void offAnimiation(uint8_t index);

    void normal(uint8_t index, uint32_t lvl, uint32_t maxLvlAvg);

    void centre(uint8_t index, uint32_t lvl, uint32_t maxLvlAvg);

    // void pulse(uint8_t index, uint32_t lvl, uint32_t maxLvlAvg, bool onlyPeak);
    void circle(uint8_t index, uint32_t lvl, uint32_t maxLvlAvg, uint16_t width, uint16_t bars, float moveSpeed,
                bool reverseOnPeak);

private:
    Adafruit_NeoPixel pixels;

    uint16_t maxLength;
    StripData first;
    StripData second;
    StripData third;
    StripData fourth;

    uint16_t selectA;
    uint16_t selectB;

    StripData *selectStrip(uint8_t index);

    void updateColorOffset(StripData *current);
};
