#include "MultiplexedStrip.hpp"

#include <Adafruit_NeoPixel.h>

#include <Normal.cpp>
#include <Circle.cpp>

#include "Core.hpp"

Animations::StripData *MultiplexedStrip::selectStrip(const uint8_t index) {
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
    first = {ledCount1, false, true, false, 400, 3000, 255};
    second = {ledCount2, false, true, false, 400, 3000, 255};
    third = {ledCount3, false, true, false, 400, 3000, 255};
    fourth = {ledCount4, false, true, false, 400, 3000, 255};
}

void MultiplexedStrip::begin() {
    pixels.begin();
    pixels.setBrightness(255);
}

void MultiplexedStrip::test(const uint8_t index) {
    const Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;

    pixels.setBrightness(30);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 255));
        pixels.show();
        delay(2);
    }
    pixels.show();
    delay(100);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255, 0, 0));
        pixels.show();
        delay(2);
    }
    pixels.show();
    delay(100);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 0));
        pixels.show();
        delay(2);
    }
    pixels.show();
    delay(100);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 255));
        pixels.show();
        delay(2);
    }
    pixels.show();
    delay(100);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255, 255, 0));
        pixels.show();
        delay(2);
    }
    pixels.show();
    delay(100);

    const auto rand = random(0, 65535);
    const auto randColor = Adafruit_NeoPixel::ColorHSV(rand);
    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, randColor);
        pixels.show();
        delay(2);
    }
    delay(100);

    pixels.clear();
    pixels.show();
    delay(100);
}

void MultiplexedStrip::updateColorOffset(Animations::StripData *current) {
    current->colorOffset += current->colorChangeSpeed;
    if (current->colorOffset >= 65535) {
        current->colorOffset = 0;
    }
}

void MultiplexedStrip::setReversed(const uint8_t index, const bool reverse) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;

    current->reversed = reverse;
}

void MultiplexedStrip::setMaxBrightness(const long brightness) {
    pixels.setBrightness(brightness);
}

void MultiplexedStrip::centre(const uint8_t index, const uint32_t lvl) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;
    updateColorOffset(current);
    Animations::renderCircle(pixels.getPixels(), current, lvl);

    // pixels.setBrightness(lvl, maxLvlAvg);
    pixels.show();
}


void MultiplexedStrip::normal(const uint8_t index, const uint32_t lvl) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;
    updateColorOffset(current);

    auto p = pixels.getPixels();
    for (int i = 0; i < pixels.numPixels() * 3; i++) {
        Console::print(p[i]);
        Console::print(" ");
    }
    Console::println("");
    Animations::renderNormal(pixels.getPixels(), current, lvl);
    for (int i = 0; i < pixels.numPixels() * 3; i++) {
        Console::print(p[i]);
        Console::print(" ");
    }
    Console::println("");
    delay(2000);
    return;

    // pixels.setBrightness(lvl, maxLvlAvg);
    pixels.show();
}

void MultiplexedStrip::offAnimation(const uint8_t index) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;

    // clearRgbPixels(pixels.getPixels(), pixels.numPixels());
    // pixels.setBrightness(30);
    // current->offAnimState.circle_position += 0.03;
    // const uint16_t start = static_cast<uint16_t>(current->offAnimState.circle_position) % current->ledCount;
    // const uint32_t color = Adafruit_NeoPixel::Color(200, 0, 70);
    // pixels.getPixels()[start] = color;
    // pixels.getPixels()[(start + current->ledCount - 1) % current->ledCount] = color;
    // pixels.show();
}
