#include "Util.hpp"

/// Maps a index x in range 0 to ledCount to a color in the range 0 to 65535 as a hue
/// 
/// Usage: Adafruit_NeoPixel::ColorHSV(colorWheel(x, ledCount, globalState.colorOffset), 255, 255);
/// @param x 0 to ledCount
/// @param ledCount max index
/// @param colorOffset offset added to the value
/// @param parts number of times to repeat the color wheel
/// @return uint32_t: 0 to 65535
uint16_t colorWheel(uint16_t x, uint16_t ledCount, uint16_t colorOffset, uint16_t parts)
{
	return (map(x, 0, ledCount, 0, 65535 * parts) + colorOffset) % 65535;
}