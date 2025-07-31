#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void Strip::test()
{
    clear();
    pixels.setPixelColor(0, Adafruit_NeoPixel::Color(100, 0, 200));
    pixels.setPixelColor(1, Adafruit_NeoPixel::Color(200, 40, 0));
    pixels.setPixelColor(2, Adafruit_NeoPixel::Color(100, 0, 200));
    pixels.setPixelColor(3, Adafruit_NeoPixel::Color(200, 40, 0));

    pixels.setPixelColor(ledCount - 1, Adafruit_NeoPixel::Color(80, 40, 200));
    pixels.setPixelColor(ledCount - 2, Adafruit_NeoPixel::Color(180, 40, 100));
    pixels.setPixelColor(ledCount - 3, Adafruit_NeoPixel::Color(240, 0, 0));
    pixels.show();
}
