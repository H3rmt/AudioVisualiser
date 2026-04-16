#include "Animations.hpp"
#include "AnimationsUtil.hpp"

void Animations::renderCircle(uint32_t *pixels, const StripData *config, const uint32_t lvl) {
    AnimationsUtil::clearRgbPixels(pixels, config->ledCount);
    const uint32_t halfLeds = config->ledCount / 2;
    const uint32_t coloredPixels = AnimationsUtil::map(lvl, UINT32_MAX, halfLeds);
    if (!config->rainbow) {
        if (config->reversed) {
            AnimationsUtil::fillRgbPixels(pixels + halfLeds - coloredPixels, coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
            AnimationsUtil::fillRgbPixels(pixels + halfLeds, coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
        } else {
            AnimationsUtil::fillRgbPixels(pixels, coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
            AnimationsUtil::fillRgbPixels(pixels + config->ledCount - coloredPixels, coloredPixels,
                                          AnimationsUtil::ColorH(config->colorOffset % UINT16_MAX));
        }
    } else if (config->reversed) {
        for (uint32_t i = 0; i < coloredPixels; i++) {
            pixels[i] = AnimationsUtil::ColorH(i * 5000 + config->colorOffset % UINT16_MAX);
        }
        for (uint32_t i = config->ledCount; i > config->ledCount - coloredPixels; i--) {
            pixels[i] = AnimationsUtil::ColorH(i * 5000 + config->colorOffset % UINT16_MAX);
        }
    } else {
        for (uint32_t i = coloredPixels; i > 0; i--) {
            pixels[i] = AnimationsUtil::ColorH(i * 5000 + config->colorOffset % UINT16_MAX);
        }
        for (uint32_t i = halfLeds; i < halfLeds + coloredPixels; i++) {
            pixels[i] = AnimationsUtil::ColorH(i * 5000 + config->colorOffset % 65535);
        }
    }
}
