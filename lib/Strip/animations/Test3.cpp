#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void Strip::test3()
{
    pixels.clear();
    pixels.setBrightness(maxBrightness);
    for (int i = 0; i < ledCount; i++)
    {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255, 0, 255));
    }
    pixels.show();
    delay(200);
    for (int i = 0; i < 255 * 2; i += 10)
    {
        pixels.setBrightness(i % maxBrightness);
        for (int ii = 0; ii < ledCount; ii++)
        {
            switch (ii % 9)
            {
            case 0:
                pixels.setPixelColor(ii, Adafruit_NeoPixel::Color(255, 0, 0));
                break; // Red
            case 1:
                pixels.setPixelColor(ii, Adafruit_NeoPixel::Color(0, 255, 0));
                break; // Green
            case 2:
                pixels.setPixelColor(ii, Adafruit_NeoPixel::Color(0, 0, 255));
                break; // Blue
            case 3:
                pixels.setPixelColor(ii, Adafruit_NeoPixel::Color(255, 165, 0));
                break; // Orange
            case 4:
                pixels.setPixelColor(ii, Adafruit_NeoPixel::Color(0, 255, 255));
                break; // Cyan
            case 5:
                pixels.setPixelColor(ii, Adafruit_NeoPixel::Color(255, 255, 0));
                break; // Yellow
            case 6:
                pixels.setPixelColor(ii, Adafruit_NeoPixel::Color(255, 0, 255));
                break; // Magenta
            case 7:
                pixels.setPixelColor(ii, Adafruit_NeoPixel::Color(255, 255, 255));
                break; // White
            case 8:
                pixels.setPixelColor(ii, Adafruit_NeoPixel::Color(128, 0, 128));
                break; // Purple
            }
        }
        pixels.show();
        delay(100);
        Serial.print(i);
        Serial.print(" ");
    }
    Serial.println();
    clear();
}
