#pragma once

#include <TFT_eSPI.h>

#include "Defs.hpp"
#include "Structs.hpp"

void drawTopBar(TFT_eSPI &tft);
void drawSideBar(TFT_eSPI &tft);
void drawFPS(TFT_eSPI &tft, float divider, double fps, double rps);

void drawSpriteIndizes(TFT_eSprite &sprite, uint8_t peakFreaquencyIndexFloat, uint8_t peakFreaquencyIndex, uint8_t peakFreaquencyIndexLazy);
void drawSpriteBars(TFT_eSprite &canvas, int approxBuffer[SAMPLES_USABLE], int peakBuffer[SAMPLES_USABLE]);
void drawSpriteAudio(TFT_eSprite &canvas, short streamBuffer[SAMPLES], bool off);
void drawDebugBars(TFT_eSprite &canvas, AnalyzeData *displayAnalyzeData, int approxBuffer[SAMPLES]);