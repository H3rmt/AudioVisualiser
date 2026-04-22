#include "Animations.hpp"
#include "AnimationsUtil.hpp"

void Animations::renderCircle(uint8_t *pixels, const StripData *config, const uint32_t lvl) {
    AnimationsUtil::clearRgbPixels(pixels, config->ledCount);
    uint32_t halfLeds = config->ledCount / 2;
    if (config->ledCount % 2 != 0) {
        halfLeds++;
    }
    const uint32_t coloredPixels = AnimationsUtil::map(lvl, UINT32_MAX, halfLeds);
    if (!config->rainbow) {
        if (config->reversed) {
            AnimationsUtil::fillRgbPixels(pixels + ((halfLeds - coloredPixels) * 3u), coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
            AnimationsUtil::fillRgbPixels(pixels + (halfLeds * 3u), coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
        } else {
            AnimationsUtil::fillRgbPixels(pixels, coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
            AnimationsUtil::fillRgbPixels(pixels + ((config->ledCount - coloredPixels) * 3u), coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
        }
    } else if (config->reversed) {
        for (uint32_t i = coloredPixels; i > 0; i--) {
            const uint32_t color = AnimationsUtil::ColorH(i * config->perLedColorChange + config->colorOffset % UINT16_MAX);
            AnimationsUtil::fillRgbPixels(pixels + (i * 3u), 1, color);
        }
        for (uint32_t i = halfLeds; i < halfLeds + coloredPixels; i++) {
            const uint32_t color = AnimationsUtil::ColorH(i * config->perLedColorChange + config->colorOffset % 65535);
            AnimationsUtil::fillRgbPixels(pixels + (i * 3u), 1, color);
        }
    } else {
        for (uint32_t i = 0; i < coloredPixels; i++) {
            const uint32_t color = AnimationsUtil::ColorH(i * config->perLedColorChange + config->colorOffset % UINT16_MAX);
            AnimationsUtil::fillRgbPixels(pixels + (i * 3u), 1, color);
        }
        for (uint32_t i = config->ledCount; i > config->ledCount - coloredPixels; i--) {
            const uint32_t color = AnimationsUtil::ColorH(i * config->perLedColorChange + config->colorOffset % UINT16_MAX);
            AnimationsUtil::fillRgbPixels(pixels + (i * 3u), 1, color);
        }
    }
}
