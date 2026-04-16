#include "Util.hpp"

#include <algorithm>

// If 'spectrum' is in the range 0-159 it is converted to a spectrum colour
// from 0 = red through to 127 = blue to 159 = violet
uint16_t rainbowColor(uint8_t spectrum, const bool wrap) {
    if (wrap) {
        spectrum = spectrum % 192;
    } else {
        spectrum = std::min(spectrum, static_cast<uint8_t>(159));
    }

    uint8_t red = 0; // Red is the top 5 bits of a 16 bit colour spectrum
    uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
    uint8_t blue = 0; // Blue is the bottom 5 bits

    uint8_t sector = spectrum >> 5;
    uint8_t amplit = spectrum & 0x1F;

    switch (sector) {
        case 0:
            red = 0x1F;
            green = amplit; // Green ramps up
            blue = 0;
            break;
        case 1:
            red = 0x1F - amplit; // Red ramps down
            green = 0x1F;
            blue = 0;
            break;
        case 2:
            red = 0;
            green = 0x1F;
            blue = amplit; // Blue ramps up
            break;
        case 3:
            red = 0;
            green = 0x1F - amplit; // Green ramps down
            blue = 0x1F;
            break;
        case 4:
            red = amplit; // Red ramps up
            green = 0;
            blue = 0x1F;
            break;
        case 5:
            red = 0x1F;
            green = 0;
            blue = 0x1F - amplit; // Blue ramps down
            break;
    }

    return red << 11 | green << 6 | blue;
}

uint16_t rgbTo565(const uint8_t r, const uint8_t g, const uint8_t b) {
    return (r & 0xF8) << 8 | (g & 0xFC) << 3 | b >> 3;
}
