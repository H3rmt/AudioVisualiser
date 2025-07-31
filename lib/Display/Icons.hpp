#pragma once

#include <TFT_eSPI.h>

void drawPauseIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color);
void drawPlayIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color);
void drawSkipIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color);
void drawConnectIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color);
void drawSettingsIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color);