#pragma once

#include <Arduino.h>

void updateMaxBright(uint8_t maxBrightness);
void drawLEDs(uint16_t input, uint16_t avg);
void drawLEDsOff();
void setupLeds();
void testLeds(int index);
void testLedsSingle();
void start();