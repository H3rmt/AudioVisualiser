#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void Strip::offAnimiation()
{
    clear();
    uint16_t start = ((uint16_t)(offAnimState.circle_position)) % ledCount;
    pixels.setPixelColor(start, Adafruit_NeoPixel::Color(200, 0, 0));
    pixels.setPixelColor((start + ledCount - 1) % ledCount, Adafruit_NeoPixel::Color(200, 0, 0));
    pixels.show();
    offAnimState.circle_position += 0.01;
}