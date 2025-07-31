#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "Util.hpp"

void Strip::normal(uint32_t lvl, uint32_t maxLvlAvg)
{
    uint16_t height = map(lvl, 0, maxLvlAvg, 0, ledCount);
    updateColorOffset();

    for (uint16_t i = 0; i < ledCount; i++)
    {
        uint32_t color;
        if (i >= height)
            color = Adafruit_NeoPixel::Color(0, 0, 0); // Off
        else if (rainbow)
            color = Adafruit_NeoPixel::ColorHSV(colorWheel(i, ledCount, globalState.colorOffset));
        else
            color = Adafruit_NeoPixel::Color(0, 0, 255); // Blue

        int j = reversed ? (ledCount - 1 - i) : i;
        pixels.setPixelColor(j, color);
    }

    setBrightness(lvl, maxLvlAvg);
    pixels.show();
}