#pragma once

#include <Arduino.h>

uint16_t rainbowColor(uint8_t spectrum, bool wrap);

uint16_t rgbTo565(uint8_t r, uint8_t g, uint8_t b);
