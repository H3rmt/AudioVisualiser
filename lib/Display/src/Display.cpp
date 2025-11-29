#include "Display.hpp"

#include <Util.hpp>

void Display::Display::init(const bool startAnimation) {
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

    Console::println("creating sprite");
    spr.setColorDepth(16);
    sptr = static_cast<uint16_t *>(spr.createSprite(spriteWidth, spriteHeight));

    if (startAnimation) {
        delay(200);
        spr.fillSprite(TFT_OLIVE);
        spr.fillSmoothRoundRect(0, 0, 30, spriteHeight, 5,TFT_DARKGREEN);
        spr.fillSmoothRoundRect(spriteWidth - 32, 0, 30, 60, 5,TFT_BLACK);
        spr.fillSmoothRoundRect(spriteWidth - 31, 70, 30, 60, 5,TFT_RED);
        spr.fillSmoothRoundRect(spriteWidth - 30, 140, 30, 50, 5,TFT_BLUE);
        addInfoString("display started");
        delay(1000);
    } else {
        delay(200);
    }
}


void Display::Display::drawBars() {
    Console::println("drawing topbar");
    drawTopBar();
}

void Display::Display::draw(const AnalyzeData *data) {
    // simple horizontal bouncing circle at the bottom of the sprite
    static int cx = 0;
    static int cx2 = 50;
    static int dx = 16;
    static int dx2 = 10;
    // static int dx = 4;
    const int radius = 10;
    const int cy = spriteHeight - radius - 2;

    if (cx == 0) cx = radius + 2;

    // clear only the bottom area where the circle moves to avoid erasing top info
    const int clearHeight = radius * 2 + 4;

    // tft.fillRect(30, spriteY + spriteHeight - clearHeight, spriteWidth - 60, clearHeight, TFT_BLACK);

    // draw the moving circle
    // tft.fillCircle(30 + cx, spriteY + cy, radius, TFT_CYAN);
    // tft.fillCircle(spriteWidth - cx2 - 30, spriteY + cy, radius, TFT_ORANGE);

    spr.fillRect(30, spriteHeight - clearHeight, spriteWidth - 60, clearHeight, TFT_BLACK);

    // draw the moving circle
    spr.fillCircle(30 + cx, cy, radius, TFT_CYAN);
    spr.fillCircle(spriteWidth - cx2 - 30, cy, radius, TFT_ORANGE);

    // step and bounce
    cx += dx;
    cx2 += dx2;
    if (cx2 - radius <= 0 || cx2 + radius >= spriteWidth - 60) {
        dx2 = -dx2;
        cx2 += dx2;
    }
    if (cx - radius <= 0 || cx + radius >= spriteWidth - 60) {
        dx = -dx;
        cx += dx;
    }


    spr.fillSmoothRoundRect(spriteWidth - 31, 70, 30, 60, 5, rainbowColor(random(0, 192)));
    dmaWrite();
}


void Display::Display::dmaWait() {
    if (dma)
        tft.dmaWait();
}

void Display::Display::dmaWrite() {
    tft.startWrite();
    if (dma)
        tft.pushImageDMA(spriteX, spriteY, spriteWidth, spriteHeight, sptr);
    else
        tft.pushImage(spriteX, spriteY, spriteWidth, spriteHeight, sptr);
    tft.endWrite();
}


void Display::Display::drawTopBar() {
    tft.setFreeFont(&FreeSans18pt7b);
    tft.setCursor(0, 35);
    tft.print("Wuzhi Audio");
    tft.setFreeFont(nullptr);

    const String str2 = String(Consts::SamplingFrequency / 1000) + " kHz";
    tft.setCursor(200, 5);
    tft.print(str2);

    const String str3 = String(Consts::Samples) + " samples";
    tft.setCursor(200, 15);
    tft.print(str3);

    tft.drawLine(270, 0, 270, TOPBARHEIGHT - 1, TFT_WHITE);
    // drawSettingsIcon(Display::tft, Display::tft.width() - 23, 23, TFT_WHITE);

    tft.drawLine(0, TOPBARHEIGHT - 1, FULLWIDTH, TOPBARHEIGHT - 1, TFT_WHITE);
}

// fps, 1000.0 / shared->millisForOneFFT, displayAnalyzeData->loudnessDivider
void Display::Display::updateFPS(const float loudnessDivider, const double framesPerSecond,
                                 const double ledsUpdatesPerSecond) {
    tft.setTextColor(TFT_WHITE, TFT_CASET);
    tft.setCursor(200, 15);
    tft.print(loudnessDivider, 2);
    tft.print(" DIV   ");

    tft.setCursor(200, 25);
    tft.print(framesPerSecond, 2);
    tft.print(" FPS");

    tft.setCursor(200, 35);
    tft.print(ledsUpdatesPerSecond, 2);
    tft.print(" UPS");
}

void Display::Display::addInfoString(const char *infoString, const bool replace) {
    dmaWait();
    if (!replace)
        messageCount++;

    Console::print("adding: ");
    Console::println(infoString);

    spr.setTextFont(4);
    spr.setTextColor(TFT_WHITE, TFT_OLIVE);
    spr.setCursor(35, 25 * messageCount - 17);
    spr.print(infoString);
    dmaWrite();
}
