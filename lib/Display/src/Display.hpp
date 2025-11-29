#pragma once

#include <TFT_eSPI.h>

#include "Core.hpp"

#define FULLWIDTH 320
// #define SIDEBARWIDTH 50
// #define SPRITEWIDTH 270
#define FULLHEIGHT 240
#define TOPBARHEIGHT 50
// #define SPRITEHEIGHT 190

#define FFT_SCALE 1100
// Scale factor for the trace in the sprite
#define TRACE_SCALE 170

// Pixelwidth of one bar in the sprite
#define WIDTH_BAR 4
// Width of the line in the trace in sprite
#define WIDTH_TRACE 1


// 0 to MAXAVGFREQ Frequencies are used to calculate the peak frequency
// #define MAXAVGFREQ (SAMPLES_USABLE / 2)


namespace Display {
    constexpr int fullWidth = 320;
    constexpr int fullHeight = 240;
    constexpr int spriteX = 0;
    constexpr int spriteY = 50;
    constexpr int spriteWidth = 320;
    constexpr int spriteHeight = 190;

    constexpr bool dma = true;

    class Display {
    public:
        /// @brief Constructor
        explicit Display() noexcept = default;

        /// initialize display and sprite
        ///
        /// @param startAnimation whether to show the animation or not
        void init(bool startAnimation);

        /// draw bars around sprite (call once)
        void drawBars();

        /// draw data into sprite and write to display
        /// @param data analyzed data to draw
        void draw(const AnalyzeData *data);

        /// update FPS counter in top bar
        /// @param loudnessDivider value of samples divider
        /// @param framesPerSecond calculated frames per second
        /// @param ledsUpdatesPerSecond calculated leds updates per second
        void updateFPS(float loudnessDivider, double framesPerSecond, double ledsUpdatesPerSecond);

        /// Adds info string to display
        /// @param infoString string to add
        /// @param replace replace last string
        void addInfoString(const char *infoString, bool replace = false);

    private:
        TFT_eSPI tft = TFT_eSPI();
        TFT_eSprite spr = TFT_eSprite(&tft);
        uint16_t *sptr = nullptr;
        int messageCount = 0;

        /// wait for last DMA to finish
        void dmaWait();

        /// start DMA write
        void dmaWrite();

        /// draw Top bar on display
        void drawTopBar();
    };
}


// void drawSpriteIndizes(uint8_t peakFreaquencyIndexFloat, uint8_t peakFreaquencyIndex,
//                        uint8_t peakFreaquencyIndexLazy);
//
// void drawSpriteBars(const int results[SAMPLES_USABLE], const int peaks[SAMPLES_USABLE]);
//
// void drawSpriteAudio(const short streamBuffer[SAMPLES], bool off);
//
// void drawDebugBars(const AnalyzeData *data, const int results[SAMPLES]);
