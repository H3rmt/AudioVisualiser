#include "Strip.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "Util.hpp"

void Strip::centre(uint32_t lvl, uint32_t maxLvlAvg)
{
    uint16_t height;
    updateColorOffset();

    // used instead of ledCount, but can vary depending on %2 on ledCount
    uint16_t count = ledCount % 2 == 0 ? ledCount / 2 : (ledCount + 1) / 2;
    height = map(lvl, 0, maxLvlAvg, 0, count);

    for (uint16_t i = 0; i < count; i++)
    {
        uint32_t color;
        if (i >= height)
            color = Adafruit_NeoPixel::Color(0, 0, 0); // Off
        else if (rainbow)
            color = Adafruit_NeoPixel::ColorHSV(colorWheel(i, count, globalState.colorOffset));
        else
            color = Adafruit_NeoPixel::Color(0, 0, 255); // Blue

        if (reversed)
        {
            pixels.setPixelColor(count - i - 1, color);
            pixels.setPixelColor(count + i - (ledCount % 2 == 0 ? 1 : 0), color);
        }
        else
        {
            pixels.setPixelColor(i, color);
            pixels.setPixelColor(ledCount - 1 - i, color);
        }
    }

    setBrightness(lvl, maxLvlAvg);
    pixels.show();
}

/*
count: ledCount: 5 => 6 / 2 = 3;
height: lvl: 8; maxLvlAvg: 10 => 0-3; 2

i: 0
0. x
1.
2.
3.
4. x

i: 1
0.
1. x
2.
3. x
4.

i: 2
0.
1.
2. x x (black)
3.
4.

# Reversed

count: ledCount: 5 => 6 / 2 = 3;
height: lvl: 8; maxLvlAvg: 10 => 0-3; 2

i: 0
0.
1.
2. xx
3.
4.

i: 1
0.
1. x
2.
3. x
4.

i: 2
0. x (black)
1.
2.
3.
4. x (black)

# Even ledCount

count: ledCount: 6 => 6 / 2 = 3;
height: lvl: 8; maxLvlAvg: 10 => 0-3; 2

i: 0
0. x
1.
2.
3.
4.
5. x

i: 1
0.
1. x
2.
3.
4. x
5.

i: 2
0.
1.
2. x (black)
3. x (black)
4.
5.

# Reversed even ledCount

count: ledCount: 6 => 6 / 2 = 3;
height: lvl: 8; maxLvlAvg: 10 => 0-3; 2

i: 0
0.
1.
2. x
3. x
4.
5.

i: 1
0.
1. x
2.
3.
4. x
5.

i: 2
0. x (black)
1.
2.
3.
4.
5. x (black)

*/