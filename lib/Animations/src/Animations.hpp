#pragma once
#include <cstdint>

namespace Animations {
    struct Rgb {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    struct CircleAnimState {
        float movement = 0.0; // movement speed
        uint32_t last_dir_change = 0; // last direction change
        uint32_t circlePosition = 0; // current position in the circle
    };

    struct OffAnimState {
        float circle_position = 0;
    };

    struct StripData {
        // fix
        uint16_t ledCount = 0;

        // configurable
        bool reversed = false;
        bool rainbow = false;
        bool adaptiveBrightness = false;
        uint16_t perLedColorChange = 0;
        uint8_t maxBrightness = 0;
        uint8_t maxHWBrightness = 0;

        // change during animation
        CircleAnimState circleAnimState;
        OffAnimState offAnimState;
    };

    void renderCentre(Rgb *pixels, const StripData *config, uint16_t lvl, uint16_t colorOffset);

    void renderNormal(Rgb *pixels, const StripData *config, uint16_t lvl, uint16_t colorOffset);
}
