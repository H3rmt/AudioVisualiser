#pragma once

#include <Arduino.h>

void updateMaxBright(uint8_t maxBrightness);
void drawLEDs(uint32_t input, uint32_t avg);
void drawLEDsOff();
void initLeds();
void testLeds();
void testLedsSingle();