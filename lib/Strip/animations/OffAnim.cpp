#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "MultiplexedStrip.hpp"

void MultiplexedStrip::offAnimation(const uint8_t index) {
    StripData *current = selectStrip(index);
    if (current == nullptr)
        return;

    pixels.clear();
    pixels.setBrightness(30);
    current->offAnimState.circle_position += 0.01;
    const uint16_t start = static_cast<uint16_t>(current->offAnimState.circle_position) % current->ledCount;
    pixels.setPixelColor(start, Adafruit_NeoPixel::Color(200, 0, 70));
    pixels.setPixelColor((start + current->ledCount - 1) % current->ledCount, Adafruit_NeoPixel::Color(200, 0, 70));
    pixels.show();
}
