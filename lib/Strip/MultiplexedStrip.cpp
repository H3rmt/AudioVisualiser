#include "MultiplexedStrip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


StripData *MultiplexedStrip::selectStrip(const uint8_t index) {
    digitalWrite(selectA, index & 0x01);
    digitalWrite(selectB, index >> 1 & 0x01);
    switch (index) {
        case 0:
            return &first;
        case 1:
            return &second;
        case 2:
            return &third;
        case 3:
            return &fourth;
        default:
            return nullptr;
    }
}

MultiplexedStrip::MultiplexedStrip(int16_t pin, uint16_t selectA, uint16_t selectB, uint16_t ledCount1,
                                   uint16_t ledCount2, uint16_t ledCount3, uint16_t ledCount4)
    : pixels(max(ledCount1, max(ledCount2, max(ledCount3, ledCount4))), pin, NEO_RGB + NEO_KHZ800),
      maxLength(max(ledCount1, max(ledCount2, max(ledCount3, ledCount4)))),
      selectA(selectA),
      selectB(selectB) {
    first = {ledCount1, false, true, false, 400, 255};
    second = {ledCount2, false, true, false, 400, 255};
    third = {ledCount3, false, true, false, 400, 255};
    fourth = {ledCount4, false, true, false, 400, 255};
}

void MultiplexedStrip::begin() {
    pixels.begin();
    pixels.setBrightness(255);
}

void MultiplexedStrip::test(const uint8_t index) {
    const StripData *current = selectStrip(index);
    if (current == nullptr)
        return;

    pixels.setBrightness(30);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 255));
    }
    pixels.show();
    delay(100);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255, 0, 0));
    }
    pixels.show();
    delay(100);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 0));
    }
    pixels.show();
    delay(100);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 255));
    }
    pixels.show();
    delay(100);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255, 255, 0));
    }
    pixels.show();
    delay(100);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255, 0, 255));
    }
    pixels.show();
    delay(100);

    pixels.clear();
    pixels.show();
    delay(100);
}

void MultiplexedStrip::updateColorOffset(StripData *current) {
    current->colorOffset += current->colorChangeSpeed;
    if (current->colorOffset >= 65535) {
        current->colorOffset = 0;
    }
}

void MultiplexedStrip::setReversed(const uint8_t index, const bool reverse) {
    StripData *current = selectStrip(index);
    if (current == nullptr)
        return;

    current->reversed = reverse;
}
