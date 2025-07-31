#include "Light.hpp"

#include "Strip.hpp"

Strip sub = Strip(140, D4);
Strip midl = Strip(63, D4);
Strip midr = Strip(62, D4);

Strip sideo = Strip(70, D5);
Strip sidem = Strip(70, D5);

void selectChannel(int channel)
{
    digitalWrite(15, (channel) & 0x01);
    digitalWrite(25, (channel >> 1) & 0x01);
}

void updateMaxBright(uint8_t maxBrightness)
{
    sub.setMaxBrightness(maxBrightness);
    midl.setMaxBrightness(maxBrightness);
    midr.setMaxBrightness(maxBrightness);
    sideo.setMaxBrightness(maxBrightness);
    sidem.setMaxBrightness(maxBrightness);
}

void drawLEDsOff()
{
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

void drawLEDs(uint32_t input, uint32_t avg)
{
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

void initLeds()
{
    sub.begin();
    sub.setAdaptiveBrightness(true);
    midl.begin();
    midl.setReversed(true);
    midr.begin();
    sideo.begin();
    sidem.begin();
    sidem.setReversed(true);
}

void testLedsSingle()
{
    sub.begin();
    sideo.begin();
    while (true)
    {
        sub.test();
        sideo.test2();
        delay(100);
        Serial.println("LEDS complete");
    }
}

void testLeds()
{
    selectChannel(0);
    sub.test();
    sideo.test();
    delay(100);
    selectChannel(1);
    midl.test();
    sidem.test();
    delay(100);
    selectChannel(2);
    midr.test();
    sideo.test();
    delay(100);
    selectChannel(3);
    sidem.test();

    delay(200);

    selectChannel(0);
    sub.test2();
    selectChannel(1);
    midl.test2();
    selectChannel(2);
    midr.test2();
    selectChannel(0);
    sideo.test2();
    selectChannel(1);
    sidem.test2();
    selectChannel(2);
    sideo.test2();
    selectChannel(3);
    sidem.test2();

    delay(200);

    selectChannel(0);
    sub.test3();
    sideo.test3();
    selectChannel(1);
    midl.test3();
    sidem.test3();
    selectChannel(2);
    midr.test3();
    sideo.test3();
    selectChannel(3);
    sidem.test3();
}