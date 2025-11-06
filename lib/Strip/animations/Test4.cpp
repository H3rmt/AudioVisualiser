#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void Strip::test4()
{
    clear();
    for (uint16_t i = 0; i < pixels.numPixels(); ++i) {
        uint16_t hue = (65535UL * i) / pixels.numPixels();
        pixels.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(hue, 255, 255));
        pixels.show();
        delay(25);
    }
    delay(2000);
    clear();
    pixels.show();
}
