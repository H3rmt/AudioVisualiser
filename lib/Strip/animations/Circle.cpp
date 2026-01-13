#include "MultiplexedStrip.hpp"

#include "Util.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void MultiplexedStrip::circle(uint8_t index, uint32_t lvl, uint32_t maxLvlAvg, uint16_t width = 5, uint16_t bars = 2, float moveSpeed = 0.1f, bool reverseOnPeak = false)
{
    StripData *current = selectStrip(index);
    if (current == nullptr)
        return;

    uint16_t steps = map(lvl, 0, maxLvlAvg, 0,  current->ledCount);
    updateColorOffset(current);
    pixels.clear();

    current->circleAnimState.movement *= 0.90;
    current->circleAnimState.last_dir_change++;

    float add = (steps * moveSpeed) + (moveSpeed / 2);
    if (add > current->circleAnimState.movement)
        current->circleAnimState.movement = add;

    if (reverseOnPeak && lvl > maxLvlAvg && current->circleAnimState.last_dir_change > 10)
    {
       current-> circleAnimState.last_dir_change = 0;
        current->reversed = !current->reversed;
    }

    current->circleAnimState.circlePosition += current->circleAnimState.movement;
    if (current->circleAnimState.circlePosition > current->ledCount)
        current->circleAnimState.circlePosition -= current->ledCount;

    uint16_t barDistance = (float)current->ledCount / bars;
    for (uint16_t i = 0; i < width; i++)
    {
        for (uint16_t bar = 0; bar < bars; bar++)
        {
            uint32_t color;
            if (current->rainbow)
                color = Adafruit_NeoPixel::ColorHSV(colorWheel(i, current->ledCount, current->colorOffset, 3));
            else
                color = Adafruit_NeoPixel::Color(0, 0, 255); // Blue
            uint16_t pos = (current->circleAnimState.circlePosition + i + (bar * barDistance)) % current->ledCount;
            if (current->reversed)
                pos = (current->ledCount - 1 - pos);
            pixels.setPixelColor(pos, color);
        }
    }

    // setBrightness(lvl, maxLvlAvg);
    pixels.show();
}