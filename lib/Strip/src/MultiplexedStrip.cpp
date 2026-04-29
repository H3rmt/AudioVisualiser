#include "MultiplexedStrip.hpp"

#include <Adafruit_NeoPixel.h>

#include <Normal.cpp>
#include <Centre.cpp>

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
    first = {ledCount1, false, true, false, static_cast<uint16_t>(UINT16_MAX / ledCount1), 255};
    second = {ledCount2, false, true, false, static_cast<uint16_t>(UINT16_MAX / ledCount2), 255};
    third = {ledCount3, false, true, false, static_cast<uint16_t>(UINT16_MAX / ledCount3), 255};
    fourth = {ledCount4, false, true, false, static_cast<uint16_t>(UINT16_MAX / ledCount4), 255};
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

void MultiplexedStrip::setMaxBrightness(const uint8_t index, const uint8_t brightness) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;
    current->maxBrightness = brightness;
}

void MultiplexedStrip::setMaxHWBrightness(const uint8_t index, const uint8_t brightness) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;
    current->maxHWBrightness = brightness;
}

void MultiplexedStrip::setReversed(const uint8_t index, const bool reverse) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;

    current->reversed = reverse;
}

void MultiplexedStrip::setPerLedColorChange(const uint8_t index, const uint16_t change) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;
    current->perLedColorChange = change;
}

void MultiplexedStrip::centre(const uint8_t index, const uint16_t lvl, const uint16_t colorOffset, const float percentMaxChangeDivider) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;
    Animations::renderCentre(reinterpret_cast<Animations::Rgb *>(pixels.getPixels()), current, lvl, colorOffset, percentMaxChangeDivider);

    pixels.show();
}

void MultiplexedStrip::circle(const uint8_t index, const uint16_t lvl, const uint16_t colorOffset, const uint16_t width,
                              const uint16_t bars, const float moveSpeed, const bool reverseOnPeak) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;
    Animations::renderCircle(reinterpret_cast<Animations::Rgb *>(pixels.getPixels()), current, lvl, colorOffset, width,
                             bars, moveSpeed, reverseOnPeak);
    pixels.show();
}


void MultiplexedStrip::normal(const uint8_t index, const uint16_t lvl, const uint16_t colorOffset) {
    const Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;

    // Console::print("pixels: ");
    // auto p = pixels.getPixels();
    // for (int i = 0; i < pixels.numPixels() * 3; i++) {
    //     Console::print(p[i]);
    //     if (i % 3 == 2)
    //         Console::print("|");
    //     Console::print(" ");
    //
    // }
    // Console::println("\nafter: ");
    Animations::renderNormal(reinterpret_cast<Animations::Rgb *>(pixels.getPixels()), current, lvl, colorOffset);
    // for (int i = 0; i < pixels.numPixels() * 3; i++) {
    //     Console::print(p[i]);
    //     Console::print(" ");
    // }
    // Console::println("\n");
    // delay(1000);

    pixels.show();
}

void MultiplexedStrip::resetOff(const uint8_t index) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;
    current->offAnimState.starting = 0;
}

void MultiplexedStrip::offAnimation(const uint8_t index) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;
    current->offAnimState.circle_position += static_cast<float>(current->ledCount) / 2000.0f;
    if (current->offAnimState.starting < 250) {
        current->offAnimState.starting++;
    }
    const uint8_t brightness_value = current->offAnimState.starting / 5;
    const uint8_t nonlinear_brightness = static_cast<uint16_t>(brightness_value) * brightness_value / 60;

    pixels.setBrightness(nonlinear_brightness);
    pixels.clear();
    int32_t start = static_cast<int16_t>(current->offAnimState.circle_position) % current->ledCount;
    if (current->reversed) {
        start = current->ledCount - 1 - start;
    }
    pixels.setPixelColor((start - 5 + current->ledCount) % current->ledCount, Adafruit_NeoPixel::Color(100, 0, 255));
    pixels.setPixelColor((start - 4 + current->ledCount) % current->ledCount, Adafruit_NeoPixel::Color(80, 0, 255));
    pixels.setPixelColor((start - 3 + current->ledCount) % current->ledCount, Adafruit_NeoPixel::Color(60, 20, 255));
    pixels.setPixelColor((start - 2 + current->ledCount) % current->ledCount, Adafruit_NeoPixel::Color(40, 40, 255));
    pixels.setPixelColor((start - 1 + current->ledCount) % current->ledCount, Adafruit_NeoPixel::Color(10, 50, 255));
    pixels.setPixelColor(start, Adafruit_NeoPixel::Color(0, 60, 255));
    pixels.setPixelColor((start + 1) % current->ledCount, Adafruit_NeoPixel::Color(10, 50, 255));
    pixels.setPixelColor((start + 2) % current->ledCount, Adafruit_NeoPixel::Color(40, 40, 255));
    pixels.setPixelColor((start + 3) % current->ledCount, Adafruit_NeoPixel::Color(60, 20, 255));
    pixels.setPixelColor((start + 4) % current->ledCount, Adafruit_NeoPixel::Color(80, 0, 255));
    pixels.setPixelColor((start + 5) % current->ledCount, Adafruit_NeoPixel::Color(100, 0, 255));
    pixels.show();
}
