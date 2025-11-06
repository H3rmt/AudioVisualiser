#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void Strip::test2()
{
    clear();
    for (int b = 0; b < 65535; b += 514)
    {
        for (int i = 0; i < ledCount; i++)
        {
            pixels.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(b % 65535, 255, 255));
        }
        pixels.show();
        delay(10);
    }
    clear();
}
