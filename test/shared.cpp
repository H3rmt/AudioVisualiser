#include <cstdint>

#include "shared.hpp"

uint16_t ColorToHue(const uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    uint32_t hue1530 = 0;

    if (r == 255 && b == 0) {
        hue1530 = g; // Red to Yellow (0-254)
    } else if (g == 255 && b == 0) {
        hue1530 = 510 - r; // Yellow to Green (255-509)
    } else if (g == 255 && r == 0) {
        hue1530 = 510 + b; // Green to Cyan (510-764)
    } else if (b == 255 && r == 0) {
        hue1530 = 1020 - g; // Cyan to Blue (765-1019)
    } else if (b == 255 && g == 0) {
        hue1530 = 1020 + r; // Blue to Magenta (1020-1274)
    } else if (r == 255 && g == 0) {
        hue1530 = 1530 - b; // Magenta to Red (1275-1530)
    }

    // Scale back to 0-65535: (hue1530 * 65536) / 1530
    return static_cast<uint16_t>(hue1530 * 65536L / 1530L);
}
