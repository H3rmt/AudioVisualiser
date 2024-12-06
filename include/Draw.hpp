#pragma once
#include <TFT_eSPI.h>

#include "Defs.hpp"

void drawTopBar(TFT_eSPI &tft);
void drawFPS(TFT_eSPI &tft, double fps, double time, float divider);
void drawSettingsIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color);
void drawSideBar(TFT_eSPI &tft);
void drawPauseIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color);
void drawPlayIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color);
void drawSkipIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color);
void drawConnectIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color);
void drawSprite(TFT_eSprite &canvas, int approxBuffer[SAMPLES_USABLE], short streamBuffer[SAMPLES], int peakBuffer[SAMPLES_USABLE], uint8_t max_index_float, u_int32_t streamBufferMax, u_int32_t avg, u_int32_t floating_avg);