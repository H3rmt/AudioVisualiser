#include "Animations.hpp"
#include "AnimationsUtil.hpp"

void Animations::renderCentre(Rgb *pixels, StripData *config, const uint16_t lvl, const uint16_t colorOffset,
                              const float percentMaxChangeDivider) {
    AnimationsUtil::clearRgbPixels(pixels, config->ledCount);
    const uint16_t halfLeds = config->ledCount / 2;
    const uint16_t targetColoredPixels = AnimationsUtil::map(lvl, UINT16_MAX, halfLeds);
    const float step = config->ledCount / percentMaxChangeDivider;
    if (config->centreAnimState.coloredPixels < targetColoredPixels) {
        config->centreAnimState.coloredPixels += step;
        if (config->centreAnimState.coloredPixels > targetColoredPixels) {
            config->centreAnimState.coloredPixels = targetColoredPixels;
        }
    } else if (config->centreAnimState.coloredPixels > targetColoredPixels) {
        config->centreAnimState.coloredPixels -= step;
        if (config->centreAnimState.coloredPixels < targetColoredPixels) {
            config->centreAnimState.coloredPixels = targetColoredPixels;
        }
    }

    const uint16_t coloredPixels = config->centreAnimState.coloredPixels;
    if (!config->rainbow) {
        const Rgb color = AnimationsUtil::ColorH(colorOffset, config->maxHWBrightness);
        if (config->reversed) {
            std::fill_n(pixels + (halfLeds - coloredPixels), coloredPixels, color);
            std::fill_n(pixels + halfLeds, coloredPixels, color);
        } else {
            std::fill_n(pixels, coloredPixels, color);
            std::fill_n(pixels + (config->ledCount - coloredPixels), coloredPixels, color);
        }
    } else if (config->reversed) {
        if (config->ledCount % 2 == 1) {
            pixels[halfLeds] = AnimationsUtil::ColorH(
                coloredPixels * config->perLedColorChange + colorOffset,
                config->maxHWBrightness);
        }
        for (uint16_t i = 0; i < coloredPixels; ++i) {
            const auto color = AnimationsUtil::ColorH(
                (coloredPixels - i) * config->perLedColorChange + colorOffset,
                config->maxHWBrightness);
            if (config->ledCount % 2 == 1) {
                pixels[halfLeds + i + 1] = color;
            } else {
                pixels[halfLeds + i] = color;
            }
            pixels[halfLeds - i - 1] = color;
        }
    } else {
        for (uint16_t i = 0; i < coloredPixels + 1; i++) {
            const auto color = AnimationsUtil::ColorH(
                (coloredPixels - i) * config->perLedColorChange + colorOffset,
                config->maxHWBrightness);
            pixels[i] = color;
            pixels[config->ledCount - i - 1] = color;
        }
    }
}
