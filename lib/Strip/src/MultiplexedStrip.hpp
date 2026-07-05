#pragma once

#include <Adafruit_NeoPixel.h>

#include "Animations.hpp"


class MultiplexedStrip {
public:
    MultiplexedStrip(
        int16_t pin,
        uint16_t ledCount1, uint16_t ledCount2,
        uint16_t ledCount3, uint16_t ledCount4
    );

    bool begin();

    void testShow(uint8_t index);

    void offAnimation(uint8_t index);

    void normal(uint8_t index, uint16_t lvl, uint16_t colorOffset, float percentMaxChangeDivider = 20);

    void centre(uint8_t index, uint16_t lvl, uint16_t colorOffset, float percentMaxChangeDivider = 20);

    void circle(uint8_t index, uint16_t lvl, uint16_t colorOffset, uint16_t width = 5, uint16_t bars = 2,
                float moveSpeed = 0.1f, bool reverseOnPeak = false);

    void waitShow() {
        // wait for write to complete
        while (!pixels.canShow());
    }

    bool canShow() {
        return pixels.canShow();
    }

    void startShow() {
        pixels.show();
    }

    void clear() {
        pixels.setBrightness(30);
        pixels.clear();
        pixels.show();
    }

    void resetOff(const uint8_t index) {
        Animations::StripData *current = selectStrip(index);
        if (current == nullptr || current->ledCount == 0)
            return;
        current->offAnimState.starting = 0;
    }

    void setMaxBrightness(const uint8_t index, const uint8_t brightness) {
        Animations::StripData *current = selectStrip(index);
        if (current == nullptr || current->ledCount == 0)
            return;
        current->maxBrightness = brightness;
    }

    void setMaxHWBrightness(const uint8_t index, const uint8_t brightness) {
        Animations::StripData *current = selectStrip(index);
        if (current == nullptr || current->ledCount == 0)
            return;
        current->maxHWBrightness = brightness;
    }

    void setRainbow(const uint8_t index, const bool rainbow) {
        Animations::StripData *current = selectStrip(index);
        if (current == nullptr || current->ledCount == 0)
            return;
        current->rainbow = rainbow;
    }

    void setReversed(const uint8_t index, const bool reverse) {
        Animations::StripData *current = selectStrip(index);
        if (current == nullptr || current->ledCount == 0)
            return;
        current->reversed = reverse;
    }

    void setPerLedColorChange(const uint8_t index, const uint16_t change) {
        Animations::StripData *current = selectStrip(index);
        if (current == nullptr || current->ledCount == 0)
            return;
        current->perLedColorChange = change;
    }

    void a(const uint8_t index) {
        const Animations::StripData *current = selectStrip(index);
        if (current == nullptr || current->ledCount == 0)
            return;

        pixels.setBrightness(30);

        for (int i = 0; i < current->ledCount; i++) {
            pixels.setPixelColor(i, Adafruit_NeoPixel::Color(90, 80, 255));
        }
        pixels.show();
    }

private:
    Adafruit_NeoPixel pixels;
    uint16_t maxLength;

    Animations::StripData first;
    Animations::StripData second;
    Animations::StripData third;
    Animations::StripData fourth;

    Animations::StripData *selectStrip(uint8_t index);
};
