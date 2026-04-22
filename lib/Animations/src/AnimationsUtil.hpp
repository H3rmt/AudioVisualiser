#pragma once

#include <algorithm>

#include "Animations.hpp"

namespace AnimationsUtil {
    inline void writeRgbPixel(uint8_t *pixels, const uint16_t index, const uint8_t r, const uint8_t g, const uint8_t b) {
        if (pixels == nullptr)
            return;
        const uint32_t base = static_cast<uint32_t>(index) * 3u;
        pixels[base] = r;
        pixels[base + 1] = g;
        pixels[base + 2] = b;
    }

    inline void colorHsvToRgb(uint16_t hue, const uint8_t sat, const uint8_t val, uint8_t &r, uint8_t &g, uint8_t &b) {
        // Remap 0-65535 to 0-1529.
        hue = (hue * 1530L + 32768) / 65536;

        if (hue < 510) {
            b = 0;
            if (hue < 255) {
                r = 255;
                g = hue;
            } else {
                r = 510 - hue;
                g = 255;
            }
        } else if (hue < 1020) {
            r = 0;
            if (hue < 765) {
                g = 255;
                b = hue - 510;
            } else {
                g = 1020 - hue;
                b = 255;
            }
        } else if (hue < 1530) {
            g = 0;
            if (hue < 1275) {
                r = hue - 1020;
                b = 255;
            } else {
                r = 255;
                b = 1530 - hue;
            }
        } else {
            r = 255;
            g = b = 0;
        }

        const uint32_t v1 = 1 + val;
        const uint16_t s1 = 1 + sat;
        const uint8_t s2 = 255 - sat;

        r = static_cast<uint8_t>((((((r * s1) >> 8) + s2) * v1) & 0xff00) >> 8);
        g = static_cast<uint8_t>((((((g * s1) >> 8) + s2) * v1) & 0xff00) >> 8);
        b = static_cast<uint8_t>((((((b * s1) >> 8) + s2) * v1) >> 8) & 0xFF);
    }

    inline void clearRgbPixels(uint8_t *pixels, const uint16_t pixelCount) {
        if (pixels == nullptr || pixelCount == 0)
            return;
        std::fill_n(pixels, static_cast<size_t>(pixelCount) * 3u, 0u);
    }

    inline void fillRgbPixels(uint8_t *pixels, const uint16_t pixelCount, const uint32_t color) {
        if (pixels == nullptr || pixelCount == 0)
            return;
        const uint8_t r = static_cast<uint8_t>((color >> 16) & 0xFFu);
        const uint8_t g = static_cast<uint8_t>((color >> 8) & 0xFFu);
        const uint8_t b = static_cast<uint8_t>(color & 0xFFu);
        for (uint16_t i = 0; i < pixelCount; ++i) {
            writeRgbPixel(pixels, i, r, g, b);
        }
    }

    inline uint32_t map(const uint32_t x, const uint32_t in_max, const uint32_t out_max) {
        return static_cast<uint64_t>(x) * static_cast<uint64_t>(out_max) / static_cast<uint64_t>(in_max);
    }

    inline uint32_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val) {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        colorHsvToRgb(hue, sat, val, r, g, b);
        return (static_cast<uint32_t>(r) << 16) |
               (static_cast<uint32_t>(g) << 8) |
               static_cast<uint32_t>(b);
    }


    inline uint32_t ColorH(uint16_t hue, uint8_t brightness = 255) {
        return ColorHSV(hue, 255, brightness);
    }
}
