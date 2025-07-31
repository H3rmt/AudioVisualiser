#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void Strip::test3()
{
    for (int i = 0; i < ledCount; i++)
    {
        pixels.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(0));
        pixels.show();
    }
    for (int i = 0; i < 1000; i++)
    {
        pixels.setBrightness(i % maxBrightness);
        delay(10);
    }
    clear();
}
