#include "Core.hpp"
#include "MultiplexedStrip.hpp"

#include "Light.h"

#include "Debug.hpp"


auto one_i = MultiplexedStrip(STRIP1OUT, 139, 0, 62, 61);
auto two_i = MultiplexedStrip(STRIP2OUT, 72, 72, 72, 72);

constexpr int SELECT1 = 14;
constexpr int SELECT2 = 15;

void setupLeds() {
    pinMode(SELECT1, OUTPUT);
    pinMode(SELECT2, OUTPUT);
    digitalWrite(SELECT1, LOW);
    digitalWrite(SELECT2, LOW);
    if (!one_i.begin()) {
        Debug::errorExit(2);
    }
    Debug::printPioUsage("after strip 1 init");
    if (!two_i.begin()) {
        Debug::errorExit(3);
    }
    Debug::printPioUsage("after strip 2 init");
}

void selectOutput(const uint8_t index) {
    digitalWrite(SELECT1, index & 0x01);
    digitalWrite(SELECT2, index >> 1 & 0x01);
}

void renderStrip(
    MultiplexedStrip &strip,
    const uint8_t index,
    const LEDSettings &settings, const uint16_t level, const uint16_t offset,
    const float divider, const uint16_t width, const uint16_t bars,
    const float speed,
    const bool reverseOnPeak
) {
    strip.setReversed(index, settings.reversed);
    strip.setMaxBrightness(index, settings.brightness);
    strip.setRainbow(index, settings.rainbow);
    strip.resetOff(index);

    switch (settings.mode) {
        case LEDMode::Normal:
            strip.normal(index, level, offset, divider);
            break;
        case LEDMode::Centre:
            strip.centre(index, level, offset, divider);
            break;
        case LEDMode::Circle:
            strip.circle(index, level, offset, width, bars, speed, reverseOnPeak);
            break;
        case LEDMode::Off:
            strip.clear();
            break;
    }
}

void updateBrightness() {
    const auto read = analogRead(26);
    const uint8_t brightness = map(read, 0, 1023, 5, 255);
    for (uint8_t index = 0; index < 4; index++) {
        one_i.setMaxHWBrightness(index, brightness);
        two_i.setMaxHWBrightness(index, brightness);
    }
}

MultiplexedStrip &one() {
    return one_i;
}

MultiplexedStrip &two() {
    return two_i;
}
