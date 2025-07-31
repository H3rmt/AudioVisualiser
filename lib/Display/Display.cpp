#include "Display.hpp"

#include <TFT_eSPI.h>

#include "Structs.hpp"
#include "DisplayDefs.hpp"
#include "AnalyzeDefs.hpp"
#include "Icons.hpp"
#include "Util.hpp"

void drawTopBar(TFT_eSPI &tft)
{
    tft.setFreeFont(&FreeSans18pt7b);
    tft.setCursor(0, 35);
    String str = String("Wuzhi Audio");
    tft.print(str);

    tft.setFreeFont(nullptr);
    String str2 = String(SAMPLING_FREQUENCY / 1000 / 4.0) + " kHz";
    tft.setCursor(200, 5);
    tft.print(str2);

    tft.setFreeFont(nullptr);
    String str3 = String((int)SAMPLES) + " samples";
    tft.setCursor(200, 15);
    tft.print(str3);

    tft.drawLine(tft.width() - 47, 0, tft.width() - 47, TOPBARHEIGHT - 2, ILI9341_WHITE);
    drawSettingsIcon(tft, tft.width() - 23, 23, ILI9341_WHITE);

    tft.drawLine(0, TOPBARHEIGHT - 2, FULLWIDTH, TOPBARHEIGHT - 2, ILI9341_WHITE);
}

void drawSideBar(TFT_eSPI &tft)
{
    tft.drawLine(tft.width() - 47, TOPBARHEIGHT, tft.width() - 47, FULLHEIGHT, ILI9341_WHITE);

    // drawPauseIcon(tft, tft.width() - 23, 75, ILI9341_WHITE);
    drawPlayIcon(tft, tft.width() - 23, 75, ILI9341_WHITE);
    // drawSkipIcon(tft, tft.width() - 23, 130, ILI9341_WHITE);
    // drawConnectIcon(tft, tft.width() - 23, 103, ILI9341_WHITE);
}

void drawFPS(TFT_eSPI &tft, float divider, double fps, double rps)
{
    tft.setCursor(200, 15);
    tft.print(divider, 2);
    tft.print(" DIV    ");

    tft.setCursor(200, 25);
    tft.print(fps, 2);
    tft.print(" FPS");

    tft.setCursor(200, 35);
    tft.print(rps, 2);
    tft.print(" RPS");
}

void clearSprite(TFT_eSprite &sprite)
{
    sprite.fillSprite(ILI9341_BLACK);
}

void drawSpriteIndizes(TFT_eSprite &sprite, uint8_t peakFreaquencyIndexFloat, uint8_t peakFreaquencyIndex, uint8_t peakFreaquencyIndexLazy)
{
    // Draw for peakFreaquencyIndexFloat (orange)
    sprite.fillRect(WIDTH_BAR * peakFreaquencyIndexFloat, 8, WIDTH_BAR - 1, 4, ILI9341_ORANGE);

    // Draw for peakFreaquencyIndex (white)
    sprite.fillRect(WIDTH_BAR * peakFreaquencyIndex, 2, WIDTH_BAR - 1, 4, ILI9341_WHITE);
    // sprite.fillRect(WIDTH_BAR * (peakFreaquencyIndex + 1), 2, WIDTH_BAR - 1, 4, ILI9341_WHITE);
    // sprite.fillRect(WIDTH_BAR * (peakFreaquencyIndex - 1), 2, WIDTH_BAR - 1, 4, ILI9341_WHITE);

    // Draw for peakFreaquencyIndexLazy (red)
    sprite.fillRect(WIDTH_BAR * peakFreaquencyIndexLazy, 12, WIDTH_BAR - 1, 4, ILI9341_RED);
}

void drawSpriteBars(TFT_eSprite &sprite, int approxBuffer[SAMPLES_USABLE], int peakBuffer[SAMPLES_USABLE])
{
    for (uint16_t i = 0; i < SAMPLES_USABLE; i++)
    {
        uint16_t hr = approxBuffer[i] / FFT_SCALE;
        if (hr > SPRITEHEIGHT)
            hr = SPRITEHEIGHT;
        if (hr < 2)
            hr = 0;
        sprite.fillRect(WIDTH_BAR * i, SPRITEHEIGHT - hr, WIDTH_BAR - 1, hr, ILI9341_WHITE);

        uint16_t hp = peakBuffer[i] / FFT_SCALE;
        if (hp > SPRITEHEIGHT)
            hp = SPRITEHEIGHT;
        if (hp < 2)
            hp = 0;
        sprite.fillRect(WIDTH_BAR * i, SPRITEHEIGHT - hp, WIDTH_BAR - 1, hp - hr, rainbowColor(127 + min(hp, 96)));
    }
}

void drawSpriteAudio(TFT_eSprite &canvas, short streamBuffer[SAMPLES], bool off)
{
    uint16_t startSample = 0;
    uint32_t color = off ? ILI9341_RED : ILI9341_GREEN;

    for (uint16_t x = 0; x < SPRITEWIDTH; x += WIDTH_TRACE)
    {
        canvas.drawLine(
            x,
            SPRITEHEIGHT / 2 - (streamBuffer[startSample] / TRACE_SCALE),
            x + WIDTH_TRACE,
            SPRITEHEIGHT / 2 - (streamBuffer[startSample + 1] / TRACE_SCALE),
            color);
        startSample++;
        if (startSample >= SAMPLES - 1)
            break;
    }
}

void drawDebugBars(TFT_eSprite &canvas, AnalyzeData *displayAnalyzeData, int approxBuffer[SAMPLES])
{
    canvas.drawLine(SPRITEWIDTH - 6, SPRITEHEIGHT / 2 + (INCREASE_DIVIDER_PEAK / TRACE_SCALE), SPRITEWIDTH, SPRITEHEIGHT / 2 + (INCREASE_DIVIDER_PEAK / TRACE_SCALE), ILI9341_ORANGE);
    canvas.drawLine(SPRITEWIDTH - 6, SPRITEHEIGHT / 2 + (DECREASE_DIVIDER_PEAK / TRACE_SCALE), SPRITEWIDTH, SPRITEHEIGHT / 2 + (DECREASE_DIVIDER_PEAK / TRACE_SCALE), ILI9341_GREEN);
    canvas.drawLine(SPRITEWIDTH - 6, SPRITEHEIGHT / 2 + (OFF_THRESHOLD / TRACE_SCALE), SPRITEWIDTH, SPRITEHEIGHT / 2 + (OFF_THRESHOLD / TRACE_SCALE), ILI9341_RED);
    canvas.drawLine(SPRITEWIDTH - 12, SPRITEHEIGHT / 2 + (displayAnalyzeData->streamBufferMaxValue / TRACE_SCALE), SPRITEWIDTH - 6, SPRITEHEIGHT / 2 + (displayAnalyzeData->streamBufferMaxValue / TRACE_SCALE), ILI9341_CYAN);

    canvas.fillRect(SPRITEWIDTH - 8, 0, 2, min(SPRITEHEIGHT, displayAnalyzeData->floatingAverage / FFT_SCALE), ILI9341_RED);
    canvas.fillRect(SPRITEWIDTH - 6, 0, 2, min(SPRITEHEIGHT, approxBuffer[displayAnalyzeData->peakFreaquencyIndex] / FFT_SCALE), ILI9341_WHITE);
    canvas.fillRect(SPRITEWIDTH - 4, 0, 2, min(SPRITEHEIGHT, displayAnalyzeData->peakFreaquencyValue / FFT_SCALE), ILI9341_PINK);
    canvas.fillRect(SPRITEWIDTH - 2, 0, 2, min(SPRITEHEIGHT, displayAnalyzeData->floatingAverageMin / FFT_SCALE), ILI9341_ORANGE);
}