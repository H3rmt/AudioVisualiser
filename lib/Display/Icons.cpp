#include "Icons.hpp"

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

    // Draw the right rectadrawSettingsIconngle
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