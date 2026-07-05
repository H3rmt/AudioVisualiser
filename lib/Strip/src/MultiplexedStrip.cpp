#include "MultiplexedStrip.hpp"

#include <Adafruit_NeoPixel.h>

#include <Normal.cpp>
#include <Centre.cpp>

#include "Core.hpp"

Animations::StripData *MultiplexedStrip::selectStrip(const uint8_t index) {
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

MultiplexedStrip::MultiplexedStrip(
    const int16_t pin,
    const uint16_t ledCount1, const uint16_t ledCount2, const uint16_t ledCount3, const uint16_t ledCount4
)
    : pixels(max(ledCount1, max(ledCount2, max(ledCount3, ledCount4))), pin, NEO_RGB + NEO_KHZ800),
      maxLength(max(ledCount1, max(ledCount2, max(ledCount3, ledCount4)))) {
    first = {ledCount1, false, true, false, static_cast<uint16_t>(UINT16_MAX / ledCount1), 255};
    second = {ledCount2, false, true, false, static_cast<uint16_t>(UINT16_MAX / ledCount2), 255};
    third = {ledCount3, false, true, false, static_cast<uint16_t>(UINT16_MAX / ledCount3), 255};
    fourth = {ledCount4, false, true, false, static_cast<uint16_t>(UINT16_MAX / ledCount4), 255};
}

bool MultiplexedStrip::begin() {
    if (!pixels.begin()) {
        return false;
    }
    pixels.setBrightness(255);
    return true;
}


void MultiplexedStrip::testShow(const uint8_t index) {
    const Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;

    pixels.setBrightness(30);

    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 255));
        pixels.show();
        delay(1);
    }
    delay(10);
    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255, 0, 0));
        pixels.show();
        delay(1);
    }
    delay(10);
    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 0));
        pixels.show();
        delay(1);
    }
    delay(10);
    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 255));
        pixels.show();
        delay(1);
    }
    delay(10);
    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255, 255, 0));
        pixels.show();
        delay(1);
    }
    delay(10);
    const auto rand = random(0, 65535);
    const auto randColor = Adafruit_NeoPixel::ColorHSV(rand);
    for (int i = 0; i < current->ledCount; i++) {
        pixels.setPixelColor(i, randColor);
        pixels.show();
        delay(1);
    }
    delay(300);

    clear();
    pixels.show();
    delay(100);
}


void MultiplexedStrip::normal(const uint8_t index, const uint16_t lvl, const uint16_t colorOffset,
                              const float percentMaxChangeDivider) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;

    Animations::renderNormal(reinterpret_cast<Animations::Rgb *>(pixels.getPixels()), current, lvl, colorOffset,
                             percentMaxChangeDivider);
}

void MultiplexedStrip::centre(const uint8_t index, const uint16_t lvl, const uint16_t colorOffset,
                              const float percentMaxChangeDivider) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;
    Animations::renderCentre(reinterpret_cast<Animations::Rgb *>(pixels.getPixels()), current, lvl, colorOffset,
                             percentMaxChangeDivider);
}

void MultiplexedStrip::circle(const uint8_t index, const uint16_t lvl, const uint16_t colorOffset, const uint16_t width,
                              const uint16_t bars, const float moveSpeed, const bool reverseOnPeak) {
    Animations::StripData *current = selectStrip(index);
    if (current == nullptr || current->ledCount == 0)
        return;
    Animations::renderCircle(reinterpret_cast<Animations::Rgb *>(pixels.getPixels()), current, lvl, colorOffset, width,
                             bars, moveSpeed, reverseOnPeak);
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
    clear();
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
}
