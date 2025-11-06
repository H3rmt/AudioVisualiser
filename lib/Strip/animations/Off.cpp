#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void Strip::off()
{
    clear();
    pixels.show();
}