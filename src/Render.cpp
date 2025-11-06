#include <Arduino.h>
#include <TFT_eSPI.h>

#include "Display.hpp"
#include "DisplayDefs.hpp"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

uint16_t *sptr = nullptr;

void displaySetup()
{
    tft.init();
    tft.setRotation(3);
    tft.initDMA();

    Serial.println("TFT started");
    tft.fillScreen(ILI9341_BLACK);
    drawTopBar(tft);
    drawSideBar(tft);

    sptr = (uint16_t *)spr.createSprite(SPRITEWIDTH, SPRITEHEIGHT);
    delay(200);
}

// Timer for FPS
long startMillis = millis();
// Frame counter
uint16_t counter = 0;

void displayUpdate(Shared *shared, AnalyzeData *data)
{
    // wait for next FFT to swap buffers
    if (!shared->newDataForDisplay)
    {
        return;
    }
    shared->newDataForDisplay = false;
    shared->allowNewDataForDisplay = false;

    tft.dmaWait();
    drawSpriteIndizes(spr, data->peakFreaquencyIndexFloat, data->peakFreaquencyIndex, data->peakFreaquencyIndexLazy);
    drawSpriteBars(spr, data->results, data->peaks);
    // drawSpriteAudio(spr, data->streamBuffer, data->off);
    drawDebugBars(spr, data, data->results);
    tft.startWrite();
    tft.pushImageDMA(4, TOPBARHEIGHT, SPRITEWIDTH, SPRITEHEIGHT, sptr);
    tft.endWrite();

    // only calculate the fps every <40> iterations.
    counter++;
    if (counter >= 40)
    {
        double fps = 40 * 1000.0 / (millis() - startMillis); // 60 frames over 12_000 - 10_000 = 2_000 => 60 * 1000 / 2_000 = 30
        startMillis = millis();

        // drawFPS(tft, fps, 1000.0 / shared->millisForOneFFT, displayAnalyzeData->loudnessDivider);
        counter = 0;
    }

    shared->allowNewDataForDisplay = true;
}