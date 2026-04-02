#pragma once

#include <Arduino.h>
#include <cstring>

uint16_t colorWheel(uint16_t x, uint16_t ledCount, uint16_t colorOffset = 0, uint16_t parts = 2);

inline void clearRgbPixels(uint8_t *pixels, const uint16_t pixelCount) {
    if (pixels == nullptr || pixelCount == 0)
        return;
    std::memset(pixels, 0, static_cast<size_t>(pixelCount) * 3u);
}

inline void fillRgbPixels(uint8_t *pixels, const uint16_t pixelCount, const uint32_t color) {
    if (pixels == nullptr || pixelCount == 0)
        return;
    std::memset(pixels, static_cast<int>(color), static_cast<size_t>(pixelCount) * 3u);
}

inline uint32_t map(const uint32_t x, const uint32_t in_min, const uint32_t in_max, const uint32_t out_min,
                    const uint32_t out_max) {
    if (in_max == in_min)
        return out_min;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
