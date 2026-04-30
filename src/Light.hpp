#pragma once

#include <Arduino.h>

#include "Core.hpp"

void updateMaxBright(uint8_t maxBrightness);

void drawLEDs(uint16_t input, uint16_t avg, const Settings *const settings);

void drawLEDsOff();

void setupLeds();

void testLeds(int index);

void testLedsSingle();

void start();
