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
    if (dma)
        tft.initDMA();
    Console::println("TFT init");
    tft.setRotation(3);

    Console::println("TFT started");
    tft.fillScreen(TFT_CASET);
    delay(250);
    tft.fillScreen(rgbTo565(20, 20, 25));

    Console::println("creating sprite");
    spr[0].setColorDepth(16);
    spr[1].setColorDepth(16);
    sprPtr[0] = static_cast<uint16_t *>(spr[0].createSprite(spriteWidth, spriteHeight));
    sprPtr[1] = static_cast<uint16_t *>(spr[1].createSprite(spriteWidth, spriteHeight));
    spr[0].fillSprite(rgbTo565(40, 40, 55));
    spr[1].fillSprite(rgbTo565(40, 40, 55));
    if (startAnimation) {
        int color = 0;
        for (int i = 0; i < spriteWidth - 30; i += 20) {
            spr[0].fillRect(15 + i, spriteHeight / 2 - 30, 15, 60, rainbowColor(color, false));
            dmaWrite(false);
            delay(30);
            color += 6;
        }
        for (int i = 0; i < spriteWidth - 30; i += 20) {
            spr[0].fillRect(15 + i, spriteHeight / 2 - 30, 15, 60, rainbowColor(color, false));
            dmaWrite(false);
            delay(30);
            color += 6;
        }
        delay(300);
        spr[0].fillSprite(rgbTo565(40, 40, 54));
    }
    settingsUI.init(settings);
    addInfoString("display started");
}

#ifndef TOUCH_CS
void Display::Display::handleTouch() {
}
#else
void Display::Display::handleTouch() {
    Timing::start(Timing::Id::DisplayHandleTouch);
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
    Timing::stop(Timing::Id::DisplayHandleTouch);
}
#endif


void Display::Display::drawBars() {
    Console::println("drawing topbar");
    drawTopBar();
}

bool Display::Display::dmaBusy() {
    if (dma)
        return tft.dmaBusy();
    return false;
}


void Display::Display::dmaWait() {
    Timing::start(Timing::Id::DisplayDmaWait);
    if (dma)
        tft.dmaWait();
    Timing::stop(Timing::Id::DisplayDmaWait);
}

void Display::Display::dmaWrite(const bool flip) {
    Timing::start(Timing::Id::DisplayDmaWrite);
    if (dma) {
        tft.startWrite();
        tft.pushImageDMA(spriteX, spriteY, spriteWidth, spriteHeight, sprPtr[sprSel]);
        tft.endWrite();
    } else
        tft.pushImage(spriteX, spriteY, spriteWidth, spriteHeight, sprPtr[sprSel]);
    if (flip)
        sprSel = !sprSel;
    Timing::stop(Timing::Id::DisplayDmaWrite);
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

void Display::Display::updateFPS(
    const uint16_t loudnessDivider,
    const uint16_t framesPerSecond,
    const uint16_t ledsPerSecond,
    const uint16_t fftsPerSecond,
    const uint16_t micDataCount
) {
    tft.setTextColor(rgbTo565(14, 145, 243), rgbTo565(20, 20, 25));
    tft.setCursor(207, 5);
    const auto uptime_seconds = millis() / 1000;
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
    tft.print("/");
    tft.print(ledsPerSecond);
    tft.print(" FPS ");

    tft.setCursor(207, 35);
    tft.print(fftsPerSecond);
    tft.print("/");
    tft.print(micDataCount);
    tft.print(" UPS ");
}

void Display::Display::addInfoString(const char *infoString, const bool replace) {
    dmaWait();
    if (!replace)
        messageCount++;

    Console::print("adding: ");
    Console::println(infoString);

    spr[0].setTextFont(4);
    spr[0].setTextColor(TFT_WHITE, rgbTo565(40, 40, 54));
    spr[0].setCursor(15, messageCount * 25 - 17);
    spr[0].print(infoString);
    dmaWrite(false);
}

void Display::Display::drawRawAudio(
    const Frame &frame,
    const float loudnessDivider,
    const bool off
) {
    Timing::start(Timing::Id::DisplayRawAudio);
    int32_t startSample = 0;
    const uint32_t color = off ? ILI9341_RED : ILI9341_GREEN;

    const float mul = 131072.0f / static_cast<float>(loudnessDivider);
    for (uint16_t x = 0; x < spriteWidth; x += 1) {
        spr[sprSel].drawLine(
            x,
            max(0, min(spriteHeight, (spriteHeight / 2) - static_cast<int16_t>(frame.samples[startSample] * mul))),
            x + 1,
            max(0, min(spriteHeight, (spriteHeight / 2) - static_cast<int16_t>(frame.samples[startSample + 4] * mul))),
            color);
        startSample += 4;
        if (startSample >= (Consts::Samples * 3) - 8)
            break;
    }
    Timing::stop(Timing::Id::DisplayRawAudio);
}


void Display::Display::drawSettings() {
    handleTouch();
    settingsUI.draw(spr[sprSel]);
}


void Display::Display::drawMain(const AnalyzedData *data, const AnalyzeDataDynamic *dynamic) {
    static int c = 0;
    c++;
    // reduce get touch calls
    if (c > 10) {
        c = 0;
        handleTouch();
    }
    Timing::start(Timing::Id::DisplayMain);

    spr[sprSel].fillSprite(rgbTo565(10, 10, 15));
    // if (sprSel) {
    //     spr[sprSel].fillRect(200, 10, 5, 5, ILI9341_WHITE);
    // } else {
    //     spr[sprSel].fillRect(220, 10, 5, 5, ILI9341_WHITE);
    // }
    for (uint16_t i = 0; i < Consts::FrequenciesUsable; i++) {
        if (i * WIDTH_BAR >= 270)
            break;

        auto hp = static_cast<uint16_t>(dynamic->peaks[i]);
        if (hp > spriteHeight)
            hp = spriteHeight;
        if (hp < 2)
            hp = 0;
        spr[sprSel].fillRect(1 + WIDTH_BAR * i, spriteHeight - hp, WIDTH_BAR, hp,
                             rainbowColor(95 + max(hp * 0.72, 20), true));

        auto hr = static_cast<uint16_t>(data->results[i]);
        if (hr > spriteHeight)
            hr = spriteHeight;
        if (hr < 2)
            hr = 0;
        spr[sprSel].fillRect(1 + WIDTH_BAR * i, spriteHeight - hr, WIDTH_BAR, hr, ILI9341_WHITE);
    }
    Timing::stop(Timing::Id::DisplayMain);
}

// max x = 320
void Display::Display::drawDebugBars(const AnalyzedData *data, const AnalyzeDataDynamic *dynamic) {
    Timing::start(Timing::Id::DisplayDebugBars);
    spr[sprSel].drawLine(270, 0, 270, spriteHeight, rgbTo565(130, 130, 130));

    spr[sprSel].fillRect(273, 0, 8, min(spriteHeight, data->peakPeakFrequencyValue), ILI9341_ORANGE);
    spr[sprSel].fillRect(283, 0, 8, min(spriteHeight, dynamic->floatingAverage), ILI9341_RED);

    const auto height = spriteHeight * (data->peakPeakValue / dynamic->floatingAverage);
    spr[sprSel].fillRect(291, spriteHeight - min(spriteHeight, height), 8, height, ILI9341_GREEN);

    spr[sprSel].fillRect(301, 0, 8, min(spriteHeight, data->peakPeakValue), ILI9341_PINK);

    // Draw for peakFreaquencyIndex (white)
    spr[sprSel].fillRect(WIDTH_BAR * data->peakPeakFrequencyIndex, 2, WIDTH_BAR, 4, ILI9341_WHITE);
    Timing::stop(Timing::Id::DisplayDebugBars);
}
