#include "Strip.hpp"

#include <Adafruit_NeoPixel.h>

void Strip::off() {
    clear();
    pixels.show();
}
