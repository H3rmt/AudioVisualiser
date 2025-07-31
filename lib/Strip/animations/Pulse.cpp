#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "Util.hpp"

void Strip::pulse(uint32_t lvl, uint32_t maxLvlAvg, bool onlyPeak = false)
{
    pulseAnimState.pulse_last = lvl;
    // to low or no real peak
    if (onlyPeak && (lvl < maxLvlAvg * 0.80 && lvl < pulseAnimState.pulse_last * 1.3))
    {
        if (pulseAnimState.isNoPeak >= 5)
        {
            for (uint16_t i = 0; i < ledCount; i++)
            {
                pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
            }
            pixels.show();
            return;
        }
        else
        {
            pulseAnimState.isNoPeak++;
        }
    }
    else
    {
        pulseAnimState.isNoPeak = 0;
    }

    // TODO check 100
    float norm = maxLvlAvg > 0 ? (float)lvl / (float)maxLvlAvg : 0.0f;
    uint32_t speed = ((uint32_t)pow(norm, 3) * 100.0 * colorChangeSpeed);
    globalState.colorOffset += speed;
    if (globalState.colorOffset >= 65535)
    {
        globalState.colorOffset = 0;
    }

    uint32_t color = Adafruit_NeoPixel::ColorHSV(globalState.colorOffset);
    for (uint16_t i = 0; i < ledCount; i++)
    {
        pixels.setPixelColor(i, color);
    }

    setBrightness(lvl, maxLvlAvg);
    pixels.show();
}