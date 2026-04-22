#pragma once
#include <cstdint>

namespace Animations {
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
        uint32_t ledCount = 0;

        // configurable
        bool reversed = false;
        bool rainbow = false;
        bool adaptiveBrightness = false;
        uint16_t colorChangeSpeed = 0;
        uint16_t perLedColorChange = 0;
        uint8_t maxBrightness = 0;

        // change during animation
        uint32_t colorOffset = 0;
        CircleAnimState circleAnimState;
        OffAnimState offAnimState;
    };

    void renderCircle(uint8_t *pixels, const StripData *config, uint32_t lvl);

    void renderNormal(uint8_t *pixels, const StripData *config, uint32_t lvl);
}
