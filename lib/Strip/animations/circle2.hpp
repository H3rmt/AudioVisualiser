#pragma once

#include "Util.hpp"


inline void renderCircle2(uint8_t *pixels, const StripData *config, const uint32_t lvl) {
    clearRgbPixels(pixels, config->ledCount);
    const uint32_t halfLeds = config->ledCount / 2;
    const uint32_t coloredPixels = map(lvl, 0, UINT32_MAX, 0, halfLeds);
    if (!config->rainbow) {
        if (config->reversed) {
            fillRgbPixels(pixels + halfLeds - coloredPixels, coloredPixels,
                          Adafruit_NeoPixel::ColorHSV(config->colorOffset % 65535, 255, 255));
            fillRgbPixels(pixels + halfLeds, coloredPixels,
                          Adafruit_NeoPixel::ColorHSV(config->colorOffset % 65535, 255, 255));
        } else {
            fillRgbPixels(pixels, coloredPixels, Adafruit_NeoPixel::ColorHSV(config->colorOffset % 65535, 255, 255));
            fillRgbPixels(pixels + config->ledCount - coloredPixels, coloredPixels,
                          Adafruit_NeoPixel::ColorHSV(config->colorOffset % 65535, 255, 255));
        }
    } else if (config->reversed) {
        for (uint32_t i = 0; i < coloredPixels; i++) {
            pixels[i] = Adafruit_NeoPixel::ColorHSV(i * 5000 + config->colorOffset % 65535, 255, 255);
        }
        for (uint32_t i = config->ledCount; i > config->ledCount - coloredPixels; i--) {
            pixels[i] = Adafruit_NeoPixel::ColorHSV(i * 5000 + config->colorOffset % 65535, 255, 255);
        }
    } else {
        for (uint32_t i = coloredPixels; i > 0; i--) {
            pixels[i] = Adafruit_NeoPixel::ColorHSV(i * 5000 + config->colorOffset % 65535, 255, 255);
        }
        for (uint32_t i = halfLeds; i < halfLeds + coloredPixels; i++) {
            pixels[i] = Adafruit_NeoPixel::ColorHSV(i * 5000 + config->colorOffset % 65535, 255, 255);
        }
    }
}
