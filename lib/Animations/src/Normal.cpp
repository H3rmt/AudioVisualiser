#include "Animations.hpp"
#include "AnimationsUtil.hpp"

void Animations::renderNormal(uint32_t *pixels, const StripData *config, const uint32_t lvl) {
    AnimationsUtil::clearRgbPixels(pixels, config->ledCount);
    const uint32_t coloredPixels = AnimationsUtil::map(lvl, UINT32_MAX, config->ledCount);
    if (!config->rainbow) {
        if (config->reversed) {
            AnimationsUtil::fillRgbPixels(pixels + (config->ledCount - coloredPixels), coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
        } else {
            AnimationsUtil::fillRgbPixels(pixels, coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
        }
    } else if (config->reversed) {
        for (uint32_t i = config->ledCount; i >= coloredPixels; i--) {
            pixels[i] = AnimationsUtil::ColorH(i * 5000 + config->colorOffset % UINT16_MAX);
        }
    } else {
        for (uint32_t i = 0; i < config->ledCount; i++) {
            pixels[i] = AnimationsUtil::ColorH(i * 5000 + config->colorOffset % UINT16_MAX);
        }
    }
}
