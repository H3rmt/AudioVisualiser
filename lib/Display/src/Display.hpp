#pragma once

#include <TFT_eSPI.h>
#include <stdint.h>

#include "Core.hpp"

#define FULLWIDTH 320
// #define SIDEBARWIDTH 50
// #define SPRITEWIDTH 270
#define FULLHEIGHT 240
#define TOPBARHEIGHT 50
// #define SPRITEHEIGHT 190

#define FFT_SCALE 100

// Pixelwidth of one bar in the sprite
#define WIDTH_BAR 2
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

    constexpr int rawDivider = 500000;
    constexpr int fftDivider = 80;

    constexpr bool dma = true;
    // constexpr bool dma = false;

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

        // TODO
        void drawDebugBars(const AnalyzeData *data);

        // TODO
        void drawDebugLines(const AnalyzeData *data);

        // TODO
        void drawRawAudio(const int32_t rawBuffer[Consts::Samples], bool off = false);

        /// update FPS counter in top bar
        /// @param loudnessDivider value of samples divider
        /// @param framesPerSecond calculated frames per second
        /// @param ledsUpdatesPerSecond calculated leds updates per second
        void updateFPS(uint16_t loudnessDivider, uint16_t framesPerSecond, uint16_t ledsUpdatesPerSecond);

        /// Adds info string to display
        /// @param infoString string to add
        /// @param replace replace last string
        void addInfoString(const char *infoString, bool replace = false);

        /// wait for last DMA to finish
        void dmaWait();

        /// start DMA write
        void dmaWrite();

        /// draw settings UI
        void drawSettingsUI();

        /// Poll touchscreen (no IRQ) and handle top-bar actions.
        void checkSettingsToggle();

        /// Whether the settings UI is currently shown instead of the audio visualiser.
        bool isSettingsMode() const { return settingsMode; }

    private:
        TFT_eSPI tft = TFT_eSPI();
        TFT_eSprite spr = TFT_eSprite(&tft);
        uint16_t *sptr = nullptr;
        uint16_t messageCount = 0;

#if defined(TOUCH_CS)
        bool touchWasDown = false;
        uint16_t lastTouchX = 0;
        uint16_t lastTouchY = 0;
        uint32_t lastTouchMillis = 0;
#endif

        bool settingsMode = false;
        uint8_t settingsPage = 0;
        static constexpr uint8_t settingsPageCount = 5;

        // Settings icon hitbox (centered at iconX/iconY, see drawTopBar()).
        static constexpr int settingsIconX = fullWidth - 23;
        static constexpr int settingsIconY = 23;
        static constexpr int settingsIconHitHalf = 20;
        static constexpr uint32_t settingsToggleCooldownMs = 1000;

        uint32_t lastSettingsToggleMillis = 0;

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
