#include "Display.hpp"

#include "Icons.hpp"
#include "SettingsUI.hpp"

#include <Util.hpp>
#include <Timing.hpp>

void Display::Display::init(const bool startAnimation, Settings *settings) {
    Console::print("starting TFT on TFT_SCLK: ");
    Console::println(TFT_SCLK);
    Console::print("TFT_MISO: ");
    Console::println(TFT_MISO);
    Console::print("TFT_MOSI: ");
    Console::println(TFT_MOSI);

    Console::println("TFT init");
    tft.init();
    Console::println("TFT init");
    tft.setRotation(3);
    tft.initDMA();

    Console::println("TFT started");
    tft.fillScreen(TFT_CASET);
    delay(250);
    tft.fillScreen(rgbTo565(20, 20, 25));

    Console::println("creating sprite");
    spr.setColorDepth(16);
    sptr = static_cast<uint16_t *>(spr.createSprite(spriteWidth, spriteHeight));
    spr.fillSprite(rgbTo565(40, 40, 55));
    if (startAnimation) {
        int color = 0;
        for (int i = 0; i < spriteWidth - 30; i += 20) {
            spr.fillRect(15 + i, spriteHeight / 2 - 30, 15, 60, rainbowColor(color, false));
            dmaWrite();
            delay(30);
            color += 6;
        }
        for (int i = 0; i < spriteWidth - 30; i += 20) {
            spr.fillRect(15 + i, spriteHeight / 2 - 30, 15, 60, rainbowColor(color, false));
            dmaWrite();
            delay(30);
            color += 6;
        }
        delay(300);
        spr.fillSprite(rgbTo565(40, 40, 54));
    }
    settingsUI.init(settings);
    addInfoString("display started");
}

void Display::Display::handleTouch() {
    Timing::start(Timing::Id::DisplayHandleTouch, 0);
    // TFT_eSPI provides calibrated screen coordinates via getTouch().
    // We treat a transition up->down as a "new touch".
    uint16_t x = 0;
    uint16_t y = 0;

    const bool down = tft.getTouch(&x, &y);
    if (down && !touchWasDown) {
        // Your touch panel reports Y inverted relative to the display coordinate system.
        y = (fullHeight - 1) - y;
        lastTouchX = x;
        lastTouchY = y;

        const bool hitSettings =
                (static_cast<int>(lastTouchX) >= (settingsIconX - settingsIconHitHalf)) &&
                (static_cast<int>(lastTouchX) <= (settingsIconX + settingsIconHitHalf)) &&
                (static_cast<int>(lastTouchY) >= (settingsIconY - settingsIconHitHalf)) &&
                (static_cast<int>(lastTouchY) <= (settingsIconY + settingsIconHitHalf));
        // Better toggle system:
        // - must release and press again (we only act on the up->down edge)
        // - must wait at least 1s between toggles
        const bool cooldownPassed = (lastSettingsToggleMillis == 0) ||
                                    ((millis() - lastSettingsToggleMillis) >= settingsToggleCooldownMs);

        if (hitSettings && cooldownPassed) {
            settingsMode = !settingsMode;
            settingsUI.resetPage();
            lastSettingsToggleMillis = millis();
            Console::println(String("settings icon pressed -> settingsMode=") + (settingsMode ? "1" : "0"));
        }

        // When in settings mode, also handle presses inside the sprite UI.
        if (settingsMode && static_cast<int>(lastTouchY) >= TOPBARHEIGHT) {
            const int sx = static_cast<int>(lastTouchX);
            const int sy = static_cast<int>(lastTouchY) - TOPBARHEIGHT;
            settingsUI.handleTouch(sx, sy, spriteWidth, spriteHeight);
        }
    }
    touchWasDown = down;
    Timing::stop(Timing::Id::DisplayHandleTouch, 0);
}


void Display::Display::drawBars() {
    Console::println("drawing topbar");
    drawTopBar();
}

void Display::Display::dmaWait() {
    Timing::start(Timing::Id::DisplayDmaWait, 0);
    if (dma)
        tft.dmaWait();
    Timing::stop(Timing::Id::DisplayDmaWait, 0);
}

void Display::Display::dmaWrite() {
    Timing::start(Timing::Id::DisplayDmaWrite, 0);
    tft.startWrite();
    if (dma)
        tft.pushImageDMA(spriteX, spriteY, spriteWidth, spriteHeight, sptr);
    else
        tft.pushImage(spriteX, spriteY, spriteWidth, spriteHeight, sptr);
    tft.endWrite();
    Timing::stop(Timing::Id::DisplayDmaWrite, 0);
}


void Display::Display::drawTopBar() {
    tft.setTextColor(TFT_WHITE, rgbTo565(20, 20, 25));
    tft.setFreeFont(&FreeSans18pt7b);
    tft.setCursor(4, 35);
    tft.print("Wuzhi Audio");
    tft.setFreeFont(nullptr);

    const String str2 = String(Consts::SamplingFrequency / 1000) + " kHz";
    tft.setCursor(207, 5);
    tft.print(str2);

    const String str3 = String(Consts::Samples) + " smpls";
    tft.setCursor(207, 15);
    tft.print(str3);

    tft.drawLine(270, 0, 270, TOPBARHEIGHT - 1, rgbTo565(130, 130, 130));
    drawSettingsIcon(tft, settingsIconX, settingsIconY, TFT_WHITE);

    tft.drawLine(0, TOPBARHEIGHT - 1, FULLWIDTH, TOPBARHEIGHT - 1, rgbTo565(130, 130, 130));
}

// fps, 1000.0 / shared->millisForOneFFT, displayAnalyzeData->loudnessDivider
void Display::Display::updateFPS(const uint16_t loudnessDivider, const uint16_t framesPerSecond,
                                 const uint16_t ledsUpdatesPerSecond, const uint32_t uptime_seconds) {
    tft.setTextColor(rgbTo565(14, 145, 243), rgbTo565(20, 20, 25));
    tft.setCursor(207, 5);
    const uint32_t hours = uptime_seconds / 3600;
    const uint32_t minutes = (uptime_seconds % 3600) / 60;
    const uint32_t seconds = uptime_seconds % 60;

    if (hours > 0) {
        tft.print(hours);
        tft.print("h ");
        tft.print(minutes);
        tft.print("m  ");
    } else {
        tft.print(minutes);
        tft.print("m ");
        tft.print(seconds);
        tft.print("s  ");
    }

    tft.setCursor(207, 15);
    tft.print(loudnessDivider);
    tft.print(" DIV    ");

    tft.setCursor(207, 25);
    tft.print(framesPerSecond);
    tft.print(" FPS ");

    tft.setCursor(207, 35);
    tft.print(ledsUpdatesPerSecond);
    tft.print(" UPS ");
}

void Display::Display::addInfoString(const char *infoString, const bool replace) {
    dmaWait();
    if (!replace)
        messageCount++;

    Console::print("adding: ");
    Console::println(infoString);

    spr.setTextFont(4);
    spr.setTextColor(TFT_WHITE, rgbTo565(40, 40, 54));
    spr.setCursor(15, messageCount * 25 - 17);
    spr.print(infoString);
    dmaWrite();
}

void Display::Display::drawRawAudio(const int16_t rawBuffer[Consts::Samples], const bool off) {
    Timing::start(Timing::Id::DisplayRawAudio, 0);
    int32_t startSample = 100;
    const uint32_t color = off ? ILI9341_RED : ILI9341_GREEN;

    for (uint16_t x = 20; x < 250; x += 1) {
        spr.drawLine(
            x,
            max(0, min(spriteHeight, (spriteHeight / 2) - (rawBuffer[startSample]))),
            x + 1,
            max(0, min(spriteHeight, (spriteHeight / 2) - (rawBuffer[startSample + 2]))),
            color);
        startSample += 2;
        if (startSample >= Consts::Samples - 2)
            break;
    }
    Timing::stop(Timing::Id::DisplayRawAudio, 0);
}


void Display::Display::drawSettings() {
    handleTouch();
    settingsUI.draw(spr);
}

int c = 0;

void Display::Display::drawMain(const AnalyzeData *data) {
    c++;
    // reduce get touch calls
    if (c > 5) {
        c = 0;
        handleTouch();
    }
    Timing::start(Timing::Id::DisplayMain, 0);

    spr.fillSprite(rgbTo565(10, 10, 15));
    const auto results = data->results;
    const auto peaks = data->peaks;

    for (uint16_t i = 0; i < Consts::FrequenciesUsable; i++) {
        if (i * WIDTH_BAR >= 270)
            break;
        uint32_t hr = results[i];
        if (hr > spriteHeight)
            hr = spriteHeight;
        if (hr < 2)
            hr = 0;
        spr.fillRect(1 + WIDTH_BAR * i, spriteHeight - hr, WIDTH_BAR, hr, ILI9341_WHITE);

        uint32_t hp = peaks[i];
        if (hp > spriteHeight)
            hp = spriteHeight;
        if (hp < 2)
            hp = 0;
        spr.fillRect(1 + WIDTH_BAR * i, spriteHeight - hp, WIDTH_BAR, hp - hr,
                     rainbowColor(95 + max(hp * 0.72, 20), true));
        // spr.fillRect(WIDTH_BAR * i, spriteHeight - hp, WIDTH_BAR, hp - hr, rainbowColor200(hp));
        // spr.fillRect(WIDTH_BAR * i, spriteHeight - hp, WIDTH_BAR, hp - hr, rainbowColor(hp));
    }
    Timing::stop(Timing::Id::DisplayMain, 0);
}

// max x = 320
void Display::Display::drawDebugBars(const AnalyzeData *data) {
    Timing::start(Timing::Id::DisplayDebugBars, 0);
    spr.drawLine(270, 0, 270, spriteHeight, rgbTo565(130, 130, 130));

    spr.fillRect(273, 0, 8, min(spriteHeight, data->floatingAverageMin), ILI9341_ORANGE);
    spr.fillRect(283, 0, 8, min(spriteHeight, data->floatingAverage), ILI9341_RED);

    const auto height = spriteHeight * (static_cast<float>(data->peakFrequencyValue) / static_cast<float>(data->
                                            floatingAverage));
    spr.fillRect(291, spriteHeight - min(spriteHeight, height), 8, height, ILI9341_GREEN);

    spr.fillRect(301, 0, 8, min(spriteHeight, data->peakFrequencyValue), ILI9341_PINK);
    spr.fillRect(311, 0, 8, min(spriteHeight, data->results[data->peakFrequencyIndex]), ILI9341_WHITE);

    // Draw for peakFreaquencyIndex (white)
    spr.fillRect(WIDTH_BAR * data->peakFrequencyIndex, 2, WIDTH_BAR, 4, ILI9341_WHITE);

    // Draw for peakFreaquencyIndexFloat (orange)
    spr.fillRect(WIDTH_BAR * data->peakFrequencyIndexFloat, 8, WIDTH_BAR, 4, ILI9341_ORANGE);

    // Draw for peakFreaquencyIndexLazy (red)
    spr.fillRect(WIDTH_BAR * data->peakFrequencyIndexLazy, 14, WIDTH_BAR, 4, ILI9341_RED);
    Timing::stop(Timing::Id::DisplayDebugBars, 0);
}


void Display::Display::drawDebugLines(const AnalyzeData *data) {
    Timing::start(Timing::Id::DisplayDebugLines, 0);
    spr.drawLine(267, spriteHeight / 2, 270, spriteHeight / 2, TFT_GREEN);

    const int off = max(0, min(spriteHeight, spriteHeight / 2 - Consts::RawMinOff));
    spr.drawLine(270 - 10, off, 270 - 3, off, TFT_RED);
#ifdef DoubleDebugLines
    const int off2 = max(0, min(spriteHeight, spriteHeight / 2 + Consts::RawMinOff));
    // spr.drawLine(270 - 10, off2, 270 - 3, off2, TFT_RED);
#endif

    const int maxV = max(0, min(spriteHeight, spriteHeight / 2 - data->rawDataMax));
    spr.drawLine(270 - 10, maxV, 270 - 3, maxV, rgbTo565(14, 145, 243));
#ifdef DoubleDebugLines
    const int maxV2 = max(0, min(spriteHeight, spriteHeight / 2 + data->rawDataMax));
    // spr.drawLine(270 - 10, maxV2, 270 - 3, maxV2, rgbTo565(14, 145, 243));
#endif

    const int inc = max(0, min(spriteHeight, spriteHeight / 2 - Consts::RawIncreaseDivider));
    spr.drawLine(270 - 10, inc, 270 - 3, inc, TFT_MAGENTA);
#ifdef DoubleDebugLines
    const int inc2 = max(0, min(spriteHeight, spriteHeight / 2 + Consts::RawIncreaseDivider));
    // spr.drawLine(270 - 10, inc2, 270 - 3, inc2, TFT_MAGENTA);
#endif

    const int div = max(0, min(spriteHeight, spriteHeight / 2 - Consts::RawDecreaseDivider));
    spr.drawLine(270 - 10, div, 270 - 3, div, TFT_ORANGE);
#ifdef DoubleDebugLines
    const int div2 = max(0, min(spriteHeight, spriteHeight / 2 + Consts::RawDecreaseDivider));
    // spr.drawLine(270 - 10, div2, 270 - 3, div2, TFT_ORANGE);
#endif
    Timing::stop(Timing::Id::DisplayDebugLines, 0);
}
