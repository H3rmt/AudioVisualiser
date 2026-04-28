#include "Animations.hpp"
#include "AnimationsUtil.hpp"

void Animations::renderNormal(Rgb *pixels, const StripData *config, const uint16_t lvl, const uint16_t colorOffset) {
    AnimationsUtil::clearRgbPixels(pixels, config->ledCount);
    const uint16_t coloredPixels = AnimationsUtil::map(lvl, UINT16_MAX, config->ledCount);
    if (!config->rainbow) {
        const Rgb color = AnimationsUtil::ColorH(colorOffset,
                                                 config->maxHWBrightness);
        if (config->reversed) {
            std::fill_n(pixels + (config->ledCount - coloredPixels), coloredPixels, color);
        } else {
            std::fill_n(pixels, coloredPixels, color);
        }
    } else if (config->reversed) {
        for (uint16_t i = 0; i < coloredPixels; i++) {
            pixels[config->ledCount - i - 1] = AnimationsUtil::ColorH(
                i * config->perLedColorChange + colorOffset,
                config->maxHWBrightness);
        }
    } else {
        for (uint16_t i = 0; i < coloredPixels; i++) {
            pixels[i] = AnimationsUtil::ColorH(i * config->perLedColorChange + colorOffset,
                                               config->maxHWBrightness);
        }
    }
}
