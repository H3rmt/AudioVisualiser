#include "Animations.hpp"
#include "AnimationsUtil.hpp"

void Animations::renderNormal(uint8_t *pixels, const StripData *config, const uint32_t lvl) {
    AnimationsUtil::clearRgbPixels(pixels, config->ledCount);
    const uint32_t coloredPixels = AnimationsUtil::map(lvl, UINT32_MAX, config->ledCount);
    if (!config->rainbow) {
        if (config->reversed) {
            AnimationsUtil::fillRgbPixels(pixels + ((config->ledCount - coloredPixels) * 3u), coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
        } else {
            AnimationsUtil::fillRgbPixels(pixels, coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
        }
    } else if (config->reversed) {
        for (uint32_t i = config->ledCount; i > coloredPixels; i--) {
            const uint32_t color = AnimationsUtil::ColorH(i * config->perLedColorChange + config->colorOffset % UINT16_MAX);
            AnimationsUtil::fillRgbPixels(pixels + ((i - 1u) * 3u), 1,
                                          color);
        }
    } else {
        for (uint32_t i = 0; i < coloredPixels; i++) {
            const uint32_t color = AnimationsUtil::ColorH(i * config->perLedColorChange + config->colorOffset % UINT16_MAX);
            AnimationsUtil::fillRgbPixels(pixels + (i * 3u), 1,
                                          color);
        }
    }
}
