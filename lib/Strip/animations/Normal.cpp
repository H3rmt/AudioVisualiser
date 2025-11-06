#include "MultiplexedStrip.hpp"

#include "Util.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void MultiplexedStrip::normal(uint8_t index, uint32_t lvl, uint32_t maxLvlAvg)
{
    StripData *current = getCurrentStrip(index);
    if (current == nullptr)
        return;

    uint16_t height = map(lvl, 0, maxLvlAvg, 0, current->ledCount);
    updateColorOffset(current);

    for (uint16_t i = 0; i < current->ledCount; i++)
    {
        uint32_t color;
        if (i >= height)
            color = Adafruit_NeoPixel::Color(0, 0, 0); // Off
        else if (current->rainbow)
            color = Adafruit_NeoPixel::ColorHSV(colorWheel(i, current->ledCount, current->colorOffset, 1));
        else
            color = Adafruit_NeoPixel::Color(0, 0, 255); // Blue

        int j = current->reversed ? (current->ledCount - 1 - i) : i;
        pixels.setPixelColor(j, color);
    }

    // setBrightness(lvl, maxLvlAvg);
    pixels.show();
}