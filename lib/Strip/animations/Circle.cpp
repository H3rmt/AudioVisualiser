#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "Util.hpp"

void Strip::circle(uint32_t lvl, uint32_t maxLvlAvg, uint16_t width = 5, uint16_t bars = 2, float moveSpeed = 0.1f, bool reverseOnPeak = false)
{
    uint16_t steps = map(lvl, 0, maxLvlAvg, 0, ledCount);
    updateColorOffset();
    clear();

    circleAnimState.movement *= 0.90;
    circleAnimState.last_dir_change++;

    float add = (steps * moveSpeed) + (moveSpeed / 2);
    if (add > circleAnimState.movement)
        circleAnimState.movement = add;

    if (reverseOnPeak && lvl > maxLvlAvg && circleAnimState.last_dir_change > 10)
    {
        circleAnimState.last_dir_change = 0;
        reversed = !reversed;
    }

    circleAnimState.circlePosition += circleAnimState.movement;
    if (circleAnimState.circlePosition > ledCount)
        circleAnimState.circlePosition -= ledCount;

    uint16_t barDistance = (float)ledCount / bars;
    for (uint16_t i = 0; i < width; i++)
    {
        for (uint16_t bar = 0; bar < bars; bar++)
        {
            uint32_t color;
            if (rainbow)
                color = Adafruit_NeoPixel::ColorHSV(colorWheel(i, ledCount, globalState.colorOffset));
            else
                color = Adafruit_NeoPixel::Color(0, 0, 255); // Blue
            uint16_t pos = (circleAnimState.circlePosition + i + (bar * barDistance)) % ledCount;
            if (reversed)
                pos = (ledCount - 1 - pos);
            pixels.setPixelColor(pos, color);
        }
    }

    setBrightness(lvl, maxLvlAvg);
    pixels.show();
}