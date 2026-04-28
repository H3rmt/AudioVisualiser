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

    one.setReversed(0, true);
    one.setReversed(2, true);
    // one.setPerLedColorChange(2, 25000);
    one.setReversed(3, true);
    // one.setPerLedColorChange(3, 25000);

    two.setReversed(0, true);
    two.setReversed(1, true);
    two.setReversed(2, true);
    // two.setReversed(2, false);
    two.setReversed(3, true);
    // two.setReversed(3, false);
}

void testLeds(const int index) {
    one.test(index);
    two.test(index);
}

void drawLEDsOff() {
    two.offAnimation(0);
    delay(1);
    two.offAnimation(1);
    delay(1);
    two.offAnimation(2);
    delay(1);
    two.offAnimation(3);
    delay(1);
}

uint16_t level = 0;
uint8_t lastBrightness  = 0;

void drawLEDs(const uint32_t input, const uint32_t avg) {
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

    // const uint32_t level = map((uint32_t) input, (uint32_t) 0, (uint32_t) avg, (uint32_t) 0, (uint32_t) UINT32_MAX);
    // const uint16_t level = (UINT16_MAX / 40) * 2;
    // const uint16_t level = (UINT16_MAX / 40) * 39;
    // const uint16_t level = (UINT16_MAX / 40) * 30;
    if (level % (UINT16_MAX / 2) < 150) {
        // delay(2000);
    }
    if (level == UINT16_MAX) {
        level = 0;
    }
    if (level > UINT16_MAX - 150)
        level = UINT16_MAX;
    else
        level += 150;

    const uint16_t offset = millis() * 12 % UINT16_MAX;

    // left front back
    two.centre(0, level, offset);
    one.centre(0, level, offset);
    delay(1);

    // right middle
    two.centre(1, level, offset);
    delay(1);

    // right front back
    two.centre(2, level, offset);
    one.centre(2, level, offset);
    delay(1);

    // left middle
    two.centre(3, level, offset);
    one.centre(3, level, offset);
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
