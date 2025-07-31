#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

/// @brief Constructor for the Strip class.
/// Initializes the NeoPixel strip with the specified pin and number of LEDs.
/// @param ledCount Number of LEDs in the strip.
/// @param ledPin Pin number for the NeoPixel strip. (GPIO pin)
Strip::Strip(uint16_t ledCount, int16_t ledPin)
    : pixels(ledCount, ledPin, NEO_RGB + NEO_KHZ800),
      ledCount(ledCount)
{
    rainbow = true;
    reversed = false;
    adaptiveBrightness = false;
    colorChangeSpeed = 400;
    maxBrightness = 255;
}

/// @brief Starts the NeoPixel strip and sets the initial brightness.
void Strip::begin()
{
    pixels.begin();
    pixels.setBrightness(maxBrightness);
}

/// @brief Reverses the LED strip direction by flipping the pixel array indexing.
/// @param reversed
void Strip::setReversed(bool reversed)
{
    this->reversed = reversed;
}

/// @brief Enables or disables changes in color over time and over the individual LEDs.
/// @param rainbow
void Strip::setRainbow(bool rainbow)
{
    this->rainbow = rainbow;
}

/// @brief Enables brightness based on the current level and average level. (more loud = more bright)
/// @param adaptive
void Strip::setAdaptiveBrightness(bool adaptive)
{
    this->adaptiveBrightness = adaptive;
}

void Strip::setMaxBrightness(uint8_t maxBrightness)
{
    this->maxBrightness = maxBrightness;
    pixels.setBrightness(maxBrightness);
}

/// @brief Sets the speed of color changes in the rainbow mode.
/// @param speed
void Strip::setColorChangeSpeed(uint16_t speed)
{
    this->colorChangeSpeed = speed;
}

/// @brief Clears the NeoPixel strip by setting all pixels to black (off).
void Strip::clear()
{
    for (uint16_t i = 0; i < ledCount; i++)
    {
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
    }
}

/// @brief Advances the color offset varaible (colorOffset) for rainbow effects.
void Strip::updateColorOffset()
{
    globalState.colorOffset += colorChangeSpeed;
    if (globalState.colorOffset >= 65535)
    {
        globalState.colorOffset = 0;
    }
}

/// @brief Sets the brightness of the NeoPixel strip based on the current level and maximum average level.
/// If adaptive brightness is enabled, it maps the level to a brightness value.
/// If adaptive brightness is disabled, it sets a fixed brightness.
/// @param lvl lvl passed to the animation as parameter
/// @param maxLvlAvg maxLvlAvg passed to the animation as parameter
void Strip::setBrightness(uint16_t lvl, uint16_t maxLvlAvg)
{
    if (!adaptiveBrightness)
        pixels.setBrightness(min(maxBrightness, 25 + ((lvl * lvl * lvl) / (maxLvlAvg * maxLvlAvg * maxLvlAvg)) * 230));
    else
        pixels.setBrightness(min(maxBrightness, 70));
}