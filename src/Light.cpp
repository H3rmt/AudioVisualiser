#include "Light.hpp"

#include "MultiplexedStrip.hpp"

auto one = MultiplexedStrip(D5, 72, 72, 72, 72);
auto two = MultiplexedStrip(D4, 2, 62, 2, 62);

void setupLeds() {
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    digitalWrite(D2, LOW);
    digitalWrite(D3, LOW);
    one.begin();
    two.begin();

    one.setReversed(0, true);
    one.setReversed(1, true);
    one.setReversed(2, true);
    one.setReversed(3, true);

    two.setReversed(1, true);
}

void testLeds(const int index) {
    one.test(index);
    two.test(index);
}

void drawLEDsOff() {
    // TODO
}

void drawLEDs(const uint32_t input, const uint32_t avg) {
    // left front back
    one.centre(0, input, avg);
    // right middle
    one.centre(1, input, avg);
    // right front back
    one.centre(2, input, avg);
    // left middle
    one.centre(3, input, avg);


    // unknown
    // two.normal(0, input, avg);
    // front right
    two.circle(1, input, avg, 8, 2, 0.12, true);
    // unknown
    // two.normal(2, input, avg);
    // front left
    two.circle(3, input, avg, 8, 2, 0.12, true);

    // sub.pulse(input, avg, false);
    // sideo.centre(input, avg);
    // midl.circle(input, avg, 4, 2, 0.12, true);
    // sidem.centre(input, avg);
    // midr.circle(input, avg, 4, 2, 0.12, true);
    // sideo.centre(input, avg);
    // sidem.centre(input, avg);
}

#ifdef BBB

void start() {
    while (true) {
        Serial.println("Starting test");
        selectChannel(0);
        one.off();
        two.off();
        selectChannel(1);
        one.off();
        two.off();
        selectChannel(2);
        one.off();
        two.off();
        selectChannel(3);
        one.off();
        two.off();
        for (uint32_t i = 0; i < 65535; i += 514) {
            auto color = Adafruit_NeoPixel::ColorHSV(i, 255, 255);
            uint32_t index = map(i, 0, 65535, 0, 70);
            Serial.println("index" + String(index));

            selectChannel(0);
            one.pixels.setPixelColor(index, color);
            one.pixels.show();
            two.pixels.setPixelColor(index, color);
            two.pixels.show();
            selectChannel(1);
            one.pixels.setPixelColor(index, color);
            one.pixels.show();
            two.pixels.setPixelColor(index, color);
            two.pixels.show();
            selectChannel(2);
            one.pixels.setPixelColor(index, color);
            one.pixels.show();
            two.pixels.setPixelColor(index, color);
            two.pixels.show();
            selectChannel(3);
            one.pixels.setPixelColor(index, color);
            one.pixels.show();
            two.pixels.setPixelColor(index, color);
            two.pixels.show();

            delay(25);
        }
        Serial.println("Finished");
        delay(500);
    }
}

void testLedsSingle() {
    delay(500);
    Serial.println("LEDS start");
    while (true) {
        selectChannel(0);
        Serial.println("Ch 1");
        one.setMaxBrightness(255); // left front back
        one.setLength(72);
        one.test4();
        one.off();
        two.setMaxBrightness(50);
        two.setLength(61); // ??
        two.test4();
        two.off();
        delay(400);

        selectChannel(1);
        Serial.println("Ch 1");
        one.setMaxBrightness(255); // right middle
        one.setLength(72);
        one.test4();
        one.off();
        two.setMaxBrightness(50);
        two.setLength(61); // front right
        two.test4();
        two.off();

        selectChannel(2);
        Serial.println("Ch 1");
        one.setMaxBrightness(255); // right front back
        one.setLength(72);
        one.test4();
        one.off();
        two.setMaxBrightness(50);
        two.setLength(61); // ??
        two.test4();
        two.off();
        delay(400);

        selectChannel(3);
        Serial.println("Ch 1");
        one.setMaxBrightness(255); // ??
        one.setLength(72);
        one.test4();
        one.off();
        two.setMaxBrightness(50);
        two.setLength(62); // front left
        two.test4();
        two.off();
        delay(400);

        Serial.println("LEDS complete");
    }
}

#endif

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
