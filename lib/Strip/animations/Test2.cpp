#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void Strip::test2()
{
    clear();
    for (int i = 0; i < ledCount; i++)
    {
        for (int b = 0; b < 65535; b += 65535 / 255) // 257
        {
            pixels.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(b));
            pixels.show();
            delay(1);
        }
    }
    clear();
}
