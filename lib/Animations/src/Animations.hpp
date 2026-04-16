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
        // set once
        uint32_t ledCount;
        bool reversed;
        bool rainbow;
        bool adaptiveBrightness;
        uint16_t colorChangeSpeed;
        uint8_t maxBrightness;

        // change
        uint32_t colorOffset = 0;
        CircleAnimState circleAnimState;
        OffAnimState offAnimState;
    };

    void renderCircle(uint32_t *pixels, const StripData *config, uint32_t lvl);

    void renderNormal(uint32_t *pixels, const StripData *config, uint32_t lvl);
}
