#include "Draw.hpp"
#include "Defs.hpp"
#include <Util.hpp>
#include <TFT_eSPI.h>

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

void drawPauseIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color)
{
    // Calculate positions for the two rectangles
    int width = 6;
    int height = 20;
    int spacing = 10;

    int halfWidth = width / 2;
    int rect1X = x - halfWidth - spacing / 2;
    int rect2X = x + spacing / 2;

    // Draw the left rectangle
    tft.fillRect(rect1X, y - height / 2, width, height, color);

    // Draw the right rectangle
    tft.fillRect(rect2X, y - height / 2, width, height, color);
}

void drawPlayIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color)
{
    int size = 17;

    // Calculate the points of the triangle
    int halfSize = size / 2;
    int x1 = x - halfSize; // Left corner
    int y1 = y - halfSize;
    int x2 = x - halfSize; // Left corner
    int y2 = y + halfSize;
    int x3 = x + halfSize; // Point of the triangle
    int y3 = y;

    // Draw the triangle
    tft.fillTriangle(x1, y1, x2, y2, x3, y3, color);
}

void drawSkipIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color)
{
}

void drawConnectIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color)
{
}

void drawSettingsIcon(TFT_eSPI &tft, uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t radius = 13;
    uint8_t border = 20;

    tft.fillRect(x - border, y - border, border * 2, border * 2, ILI9341_BLACK);
    // tft.drawRect(x - border, y - border, border * 2, border * 2, ILI9341_WHITE);

    tft.drawCircle(x, y, radius - 1, color);
    tft.drawCircle(x, y, radius / 2, color);

    // Draw the teeth of the gear
    int teeth = 8; // Number of teeth
    float angleIncrement = TWO_PI / teeth;
    float toothWidth = angleIncrement / 3;
    for (int i = 0; i < teeth; i++)
    {
        float angle = i * angleIncrement;
        float angle1 = angle - toothWidth / 2;
        float angle2 = angle + toothWidth / 2;

        int x1 = x + cos(angle1) * (radius - 1);
        int y1 = y + sin(angle1) * (radius - 1);
        int x2 = x + cos(angle2) * (radius - 1);
        int y2 = y + sin(angle2) * (radius - 1);
        int x3 = x + cos(angle2) * (radius + 6);
        int y3 = y + sin(angle2) * (radius + 6);
        int x4 = x + cos(angle1) * (radius + 6);
        int y4 = y + sin(angle1) * (radius + 6);

        // Draw the teeth as small rectangles
        tft.drawLine(x2, y2, x3, y3, color);
        tft.drawLine(x3, y3, x4, y4, color);
        tft.drawLine(x4, y4, x1, y1, color);
    }
}

void drawFPS(TFT_eSPI &tft, double fps, double time, float divider)
{
    tft.setCursor(200, 15);
    tft.print(divider, 2);
    tft.print(" DIV    ");

    tft.setCursor(200, 25);
    tft.print(fps, 2);
    tft.print(" FPS");

    tft.setCursor(200, 35);
    tft.print(time, 2);
    tft.print(" IPS");
}

void drawSprite(TFT_eSprite &canvas, int approxBuffer[SAMPLES_USABLE], short streamBuffer[SAMPLES], int peakBuffer[SAMPLES_USABLE], uint8_t max_index_float, uint8_t max_index_actual, uint8_t max_index, u_int32_t stream_buffer_max, u_int32_t floating_min, u_int32_t avg, u_int32_t floating_avg, bool off)
{
    canvas.fillSprite(ILI9341_BLACK);
    for (uint16_t i = 0; i < SAMPLES_USABLE; i++)
    {
        // Convert amplitude to y height, give higher frequencies a boost
        uint16_t hr = approxBuffer[i] / FFT_SCALE;
        if (hr > HEIGHT)
            hr = HEIGHT;
        if (hr < 2)
            hr = 0;

#if defined(DRAW_PEAK) || defined(DOT_PEAK)
        uint16_t hp = peakBuffer[i] / FFT_SCALE;
        if (hp > HEIGHT)
            hp = HEIGHT;
        if (hp < 2)
            hp = 0;
        uint16_t col = rainbowColor(127 + min(hp, 96));

#ifdef DRAW_MAX
        if (i + 1 >= max_index_actual && i - 1 <= max_index_actual)
        {
            canvas.fillRect(WIDTH_BAR * i, 2, WIDTH_BAR - 1, 4, ILI9341_WHITE);
        }
        if (i == max_index_float)
        {
            canvas.fillRect(WIDTH_BAR * i, 8, WIDTH_BAR - 1, 4, ILI9341_ORANGE);
        }
        if (i == max_index)
        {
            canvas.fillRect(WIDTH_BAR * i, 12, WIDTH_BAR - 1, 4, ILI9341_RED);
        }
#endif

#endif

#ifdef DRAW_PEAK
        canvas.fillRect(WIDTH_BAR * i, HEIGHT - hp, WIDTH_BAR - 1, hp - hr, col);
#endif
#ifdef DOT_PEAK
        canvas.fillRect(WIDTH_BAR * i, HEIGHT - hp, WIDTH_BAR - 1, 2, col);
#endif
        canvas.fillRect(WIDTH_BAR * i, HEIGHT - hr, WIDTH_BAR - 1, hr, ILI9341_WHITE);
    }

#ifdef DRAW_TRACE
    uint16_t startSample = 0;
    uint32_t color = off ? ILI9341_RED : ILI9341_GREEN;

    // Render the 'scope trace, only a half the buffer is plotted after the trigger point
    for (uint16_t x = 0; x < WIDTH; x += WIDTH_TRACE)
    {
        canvas.drawLine(
            x,
            HEIGHT / 2 - (streamBuffer[startSample] / TRACE_SCALE),
            x + WIDTH_TRACE,
            HEIGHT / 2 - (streamBuffer[startSample + 1] / TRACE_SCALE),
            color);
        startSample++;
        if (startSample >= SAMPLES - 1)
            break;
    }
    #ifdef DRAW_RED_CROSS
    for (int x = 0; x < 4; x++)
    {
        canvas.drawLine(1 + x * 64, HEIGHT / 2 - 2, 1 + x * 64 + 3, HEIGHT / 2 + 2, ILI9341_RED);
        canvas.drawLine(1 + x * 64, HEIGHT / 2 + 2, 1 + x * 64 + 3, HEIGHT / 2 - 2, ILI9341_RED);
    }
    #endif
    canvas.drawLine(WIDTH - 6, HEIGHT / 2 + (INCREASE_DIVIDER_PEAK / TRACE_SCALE), WIDTH, HEIGHT / 2 + (INCREASE_DIVIDER_PEAK / TRACE_SCALE), ILI9341_ORANGE);
    canvas.drawLine(WIDTH - 6, HEIGHT / 2 + (DECREASE_DIVIDER_PEAK / TRACE_SCALE), WIDTH, HEIGHT / 2 + (DECREASE_DIVIDER_PEAK / TRACE_SCALE), ILI9341_GREEN);
    canvas.drawLine(WIDTH - 6, HEIGHT / 2 + (OFF_THRESHOLD / TRACE_SCALE), WIDTH, HEIGHT / 2 + (OFF_THRESHOLD / TRACE_SCALE), ILI9341_RED);

    canvas.drawLine(WIDTH - 12, HEIGHT / 2 + (stream_buffer_max / TRACE_SCALE), WIDTH - 6, HEIGHT / 2 + (stream_buffer_max / TRACE_SCALE), ILI9341_CYAN);
#endif

#ifdef DRAW_INFO
    uint16_t sbm = stream_buffer_max / (TRACE_SCALE / 2);
    if (sbm > HEIGHT)
        sbm = HEIGHT;
    canvas.fillRect(WIDTH - 10, 0, 2, sbm, ILI9341_GREEN);

    uint16_t ha = avg / FFT_SCALE;
    if (ha > HEIGHT)
        ha = HEIGHT;
    canvas.fillRect(WIDTH - 8, 0, 2, ha, ILI9341_RED);

    uint16_t hav = approxBuffer[max_index_float] / FFT_SCALE;
    if (hav > HEIGHT)
        hav = HEIGHT;
    canvas.fillRect(WIDTH - 6, 0, 2, hav, ILI9341_WHITE);

    uint16_t hfa = floating_avg / FFT_SCALE;
    if (hfa > HEIGHT)
        hfa = HEIGHT;
    canvas.fillRect(WIDTH - 4, 0, 2, hfa, ILI9341_PINK);

    uint16_t hfm = floating_min / FFT_SCALE;
    if (hfm > HEIGHT)
        hfm = HEIGHT;
    canvas.fillRect(WIDTH - 2, 0, 2, hfm, ILI9341_ORANGE);
#endif
}