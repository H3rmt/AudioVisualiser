#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

struct CircleAnimState {
    float movement = 0.0; // movement speed
    uint32_t last_dir_change = 0; // last direction change
    uint32_t circlePosition = 0; // current position in the circle
};

struct OffAnimState {
    float circle_position = 0;
};

struct StripData {
    // set once
    uint32_t ledCount;
    bool reversed;
    bool rainbow;
    bool adaptiveBrightness;
    uint16_t colorChangeSpeed;
    uint8_t maxBrightness;

    // change
    uint32_t colorOffset = 0;
    CircleAnimState circleAnimState;
    OffAnimState offAnimState;
};

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

    // void setMaxBrightness(long brightness);

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

    static void updateColorOffset(StripData *current);
};
