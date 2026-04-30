#include "Light.hpp"

#include "Core.hpp"
#include "MultiplexedStrip.hpp"

constexpr int STRIP1OUT = 12;
constexpr int STRIP2OUT = 13;
constexpr int SELECT1 = 14;
constexpr int SELECT2 = 15;

auto one = MultiplexedStrip(STRIP1OUT, SELECT1, SELECT2, 139, 0, 62, 61);
auto two = MultiplexedStrip(STRIP2OUT, SELECT1, SELECT2, 72, 72, 72, 72);

void setupLeds() {
    pinMode(SELECT1, OUTPUT);
    pinMode(SELECT2, OUTPUT);
    digitalWrite(SELECT1, LOW);
    digitalWrite(SELECT2, LOW);
    one.begin();
    two.begin();
}

void testLeds(const int index) {
    one.test(index);
    two.test(index);
}

void drawLEDsOff() {
    one.offAnimation(0);
    two.offAnimation(0);
    // delay(1);
    two.offAnimation(1);
    // delay(1);
    one.offAnimation(2);
    two.offAnimation(2);
    // delay(1);
    one.offAnimation(3);
    two.offAnimation(3);
    delay(1);
}

uint16_t level = 0;
uint8_t lastBrightness = 0;

void drawLEDs(const uint16_t input, const uint16_t avg, const Settings *const settings) {
    const auto read = analogRead(26);
    if (const uint8_t brightness = map(read, 0, 1023, 5, 255); brightness != lastBrightness) {
        lastBrightness = brightness;
        one.setMaxHWBrightness(0, brightness);
        one.setMaxHWBrightness(1, brightness);
        one.setMaxHWBrightness(2, brightness);
        one.setMaxHWBrightness(3, brightness);
        two.setMaxHWBrightness(0, brightness);
        two.setMaxHWBrightness(1, brightness);
        two.setMaxHWBrightness(2, brightness);
        two.setMaxHWBrightness(3, brightness);
    }
    one.resetOff(0);
    two.resetOff(0);
    two.resetOff(1);
    one.resetOff(2);
    two.resetOff(2);
    one.resetOff(3);
    two.resetOff(3);

    one.setMaxBrightness(2, settings->frontLeft.brightness);
    one.setMaxBrightness(3, settings->frontRight.brightness);
    two.setMaxBrightness(2, settings->rightMiddle.brightness);
    two.setMaxBrightness(3, settings->rightFrontBack.brightness);

    one.setReversed(2, settings->frontLeft.reversed);
    one.setReversed(3, settings->frontRight.reversed);
    two.setReversed(2, settings->rightMiddle.reversed);
    two.setReversed(3, settings->rightFrontBack.reversed);

    const uint16_t level = input == 0 || avg == 0 ? 0 : static_cast<uint32_t>(input) * UINT16_MAX / avg;
    const uint16_t offset = millis() * 12 % UINT16_MAX;

    one.setReversed(0, settings->frontCentre.reversed);
    one.setMaxBrightness(0, settings->frontCentre.brightness);
    one.setRainbow(0, settings->frontCentre.rainbow);
    switch (settings->frontCentre.mode) {
        case LEDMode::Normal:
            one.normal(0, level, offset, 20);
            break;
        case LEDMode::Centre:
            one.centre(0, level, offset, 20);
            break;
        case LEDMode::Circle:
            one.circle(0, level, offset, 15, 3, 1.3, true);
            break;
        case LEDMode::Off:
            one.off(0);
    }
    two.setReversed(0, settings->leftFrontBack.reversed);
    two.setMaxBrightness(0, settings->leftFrontBack.brightness);
    two.setRainbow(0, settings->leftFrontBack.rainbow);
    switch (settings->leftFrontBack.mode) {
        case LEDMode::Normal:
            two.normal(0, level, offset, 5);
            break;
        case LEDMode::Centre:
            two.centre(0, level, offset, 5);
            break;
        case LEDMode::Circle:
            two.circle(0, level, offset, 15, 2, 1.0, true);
            break;
        case LEDMode::Off:
            two.off(0);
    }
    delay(1);

    // -----------------------------------------------------------------------------------------------------------------

    two.setReversed(1, settings->rightFrontBack.reversed);
    two.setMaxBrightness(1, settings->rightFrontBack.brightness);
    two.setRainbow(1, settings->rightFrontBack.rainbow);
    switch (settings->rightFrontBack.mode) {
        case LEDMode::Normal:
            two.normal(1, level, offset, 5);
            break;
        case LEDMode::Centre:
            two.centre(1, level, offset, 5);
            break;
        case LEDMode::Circle:
            two.circle(1, level, offset, 15, 2, 1.0, true);
            break;
        case LEDMode::Off:
            two.off(1);
    }
    delay(1);

    // -----------------------------------------------------------------------------------------------------------------

    one.setReversed(2, settings->frontLeft.reversed);
    one.setMaxBrightness(2, settings->frontLeft.brightness);
    one.setRainbow(2, settings->frontLeft.rainbow);
    switch (settings->frontLeft.mode) {
        case LEDMode::Normal:
            one.normal(2, level, offset, 20);
            break;
        case LEDMode::Centre:
            one.centre(2, level, offset, 20);
            break;
        case LEDMode::Circle:
            one.circle(2, level, offset, 8, 3, 1.3, true);
            break;
        case LEDMode::Off:
            one.off(2);
    }
    two.setReversed(2, settings->rightMiddle.reversed);
    two.setMaxBrightness(2, settings->rightMiddle.brightness);
    two.setRainbow(2, settings->rightMiddle.rainbow);
    switch (settings->rightMiddle.mode) {
        case LEDMode::Normal:
            two.normal(2, level, offset, 5);
            break;
        case LEDMode::Centre:
            two.centre(2, level, offset, 5);
            break;
        case LEDMode::Circle:
            two.circle(2, level, offset, 15, 2, 1.0, true);
            break;
        case LEDMode::Off:
            two.off(2);
    }
    delay(1);

    // -----------------------------------------------------------------------------------------------------------------

    one.setReversed(3, settings->frontRight.reversed);
    one.setMaxBrightness(3, settings->frontRight.brightness);
    one.setRainbow(3, settings->frontRight.rainbow);
    switch (settings->frontRight.mode) {
        case LEDMode::Normal:
            one.normal(3, level, offset, 20);
            break;
        case LEDMode::Centre:
            one.centre(3, level, offset, 20);
            break;
        case LEDMode::Circle:
            one.circle(3, level, offset, 8, 3, 1.3, true);
            break;
        case LEDMode::Off:
            one.off(3);
    }
    two.setReversed(3, settings->leftMiddle.reversed);
    two.setMaxBrightness(3, settings->leftMiddle.brightness);
    two.setRainbow(3, settings->leftMiddle.rainbow);
    switch (settings->leftMiddle.mode) {
        case LEDMode::Normal:
            two.normal(3, level, offset, 5);
            break;
        case LEDMode::Centre:
            two.centre(3, level, offset, 5);
            break;
        case LEDMode::Circle:
            two.circle(3, level, offset, 15, 2, 1.0, true);
            break;
        case LEDMode::Off:
            two.off(3);
    }
    delay(1);
}


#ifdef AAA

void updateMaxBright(uint8_t maxBrightness) {
    sub.setMaxBrightness(maxBrightness);
    midl.setMaxBrightness(maxBrightness);
    midr.setMaxBrightness(maxBrightness);
    sideo.setMaxBrightness(maxBrightness);
    sidem.setMaxBrightness(maxBrightness);
}

void drawLEDsOff() {
    selectChannel(0);
    sub.offAnimiation();
    sideo.offAnimiation();
    selectChannel(1);
    midl.offAnimiation();
    sidem.offAnimiation();
    selectChannel(2);
    midr.offAnimiation();
    sideo.offAnimiation();
    selectChannel(3);
    sidem.offAnimiation();
}

void drawLEDs(uint32_t input, uint32_t avg) {
    selectChannel(0);
    sub.pulse(input, avg, false);
    sideo.centre(input, avg);
    selectChannel(1);
    midl.circle(input, avg, 4, 2, 0.12, true);
    sidem.centre(input, avg);
    selectChannel(2);
    midr.circle(input, avg, 4, 2, 0.12, true);
    sideo.centre(input, avg);
    selectChannel(3);
    sidem.centre(input, avg);
}

void initLeds() {
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    sub.begin();
    sub.setAdaptiveBrightness(true);
    midl.begin();
    midl.setReversed(true);
    midr.begin();
    sideo.begin();
    sidem.begin();
    sidem.setReversed(true);
}
#endif
