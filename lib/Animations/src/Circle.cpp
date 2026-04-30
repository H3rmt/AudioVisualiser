#include "Animations.hpp"
#include "AnimationsUtil.hpp"

#include <Arduino.h>

void Animations::renderCircle(Rgb *pixels, StripData *config, const uint16_t lvl, const uint16_t colorOffset,
                              const uint16_t width, const uint16_t bars, const float moveSpeed,
                              const bool reverseOnPeak) {
    AnimationsUtil::clearRgbPixels(pixels, config->ledCount);

    config->circleAnimState.movement *= 0.95;
    config->circleAnimState.last_dir_change++;

    // max = 257 with moveSpeed = 1.0
    if (const float add = (((lvl / 255) + 10) * moveSpeed); add > config->circleAnimState.movement)
        config->circleAnimState.movement = add;

    if (reverseOnPeak && lvl > UINT16_MAX - 1000 && config->circleAnimState.last_dir_change > 20) {
        config->circleAnimState.last_dir_change = 0;
        config->reversed = !config->reversed;
    }

    // movement max = 257
    // 100 leds => 257 * 100 / (257 * 20) = 5
    config->circleAnimState.circlePosition += config->circleAnimState.movement * config->ledCount / (257 * 20);
    if (config->circleAnimState.circlePosition > config->ledCount)
        config->circleAnimState.circlePosition -= config->ledCount;

    const uint16_t barDistance = static_cast<float>(config->ledCount) / bars;
    for (uint16_t i = 0; i < width; i++) {
        for (uint16_t bar = 0; bar < bars; bar++) {
            Rgb color;
            if (config->rainbow)
                color = AnimationsUtil::ColorH(i * config->perLedColorChange + colorOffset, maxBright(config) - i * 10);
            else
                color = AnimationsUtil::ColorH(colorOffset, max(0, maxBright(config) - i * 10));
            uint16_t pos = static_cast<uint16_t>(config->circleAnimState.circlePosition + i + (bar * barDistance)) %
                           config->ledCount;
            if (config->reversed)
                pos = config->ledCount - 1 - pos;
            pixels[pos] = color;
        }
    }
}
