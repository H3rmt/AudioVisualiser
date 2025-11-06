#include "MultiplexedStrip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


StripData *MultiplexedStrip::getCurrentStrip(uint8_t index)
{
    digitalWrite(D2, (index) & 0x01);
    digitalWrite(D3, (index >> 1) & 0x01);
    switch (index)
    {
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

MultiplexedStrip::MultiplexedStrip(uint16_t pin, uint16_t firstLength, uint16_t secondLength, uint16_t thirdLength, uint16_t fourthLength)
    : maxLength(max(firstLength, max(secondLength, max(thirdLength, fourthLength)))),
      pixels(max(firstLength, max(secondLength, max(thirdLength, fourthLength))), pin, NEO_RGB + NEO_KHZ800)
{
    first = {firstLength, false, true, false, 400, 255};
    second = {secondLength, false, true, false, 400, 255};
    third = {thirdLength, false, true, false, 400, 255};
    fourth = {fourthLength, false, true, false, 400, 255};
}

void MultiplexedStrip::begin()
{
    pixels.begin();
    pixels.setBrightness(255);
}

void MultiplexedStrip::test(uint8_t index)
{
    StripData *current = getCurrentStrip(index);
    if (current == nullptr)
        return;

    pixels.setPixelColor(0, Adafruit_NeoPixel::Color(255, 0, 255));
    pixels.setPixelColor(1, Adafruit_NeoPixel::Color(0, 255, 255));
    pixels.show();
}

void MultiplexedStrip::updateColorOffset(StripData *current)
{
    current->colorOffset += current->colorChangeSpeed;
    if (current->colorOffset >= 65535)
    {
        current->colorOffset = 0;
    }
}

void MultiplexedStrip::setReversed(uint8_t index, bool reverse)
{
    StripData *current = getCurrentStrip(index);
    if (current == nullptr)
        return;

    current->reversed = reverse;
}