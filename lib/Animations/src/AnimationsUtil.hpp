#pragma once

#include <algorithm>
#include <cstdint>

#include "Animations.hpp"

namespace AnimationsUtil {
    inline void clearRgbPixels(Animations::Rgb *pixels, const uint16_t pixelCount) {
        if (pixels == nullptr || pixelCount == 0)
            return;
        std::fill_n(pixels, static_cast<size_t>(pixelCount), Animations::Rgb{0, 0, 0});
    }

    inline void fillRgbPixels(Animations::Rgb *pixels, const uint16_t pixelCount, const Animations::Rgb color) {
        if (pixels == nullptr || pixelCount == 0)
            return;
        std::fill_n(pixels, static_cast<size_t>(pixelCount), color);
    }

    inline uint32_t map(const uint16_t x, const uint16_t in_max, const uint16_t out_max) {
        return static_cast<uint32_t>(x) * static_cast<uint32_t>(out_max) / static_cast<uint32_t>(in_max);
    }

    inline Animations::Rgb ColorHSV(uint16_t hue, const uint8_t sat, const uint8_t val) {
        uint8_t r;
        uint8_t g;
        uint8_t b;

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

        return Animations::Rgb{r, g, b};
    }


    inline Animations::Rgb ColorH(const uint16_t hue, const uint8_t brightness = 255) {
        return ColorHSV(hue, 255, brightness);
    }
}
