#pragma once

#include <TFT_eSPI.h>

#include <stdint.h>

namespace SettingsUI {
    enum class Action : uint8_t {
        None,
        PageUp,
        PageDown,
    };

    // Draws the settings UI into the given sprite.
    // pageIndex is clamped to [0, pageCount-1].
    void draw(TFT_eSprite &spr, int pageIndex, int pageCount);

    // Returns what UI element was tapped, given sprite-local coordinates.
    // x,y are relative to the sprite (0..width-1, 0..height-1).
    Action actionForTouch(int x, int y, int spriteW, int spriteH);

    // Each page draws only the left content area (0..269). Sidebar is drawn elsewhere.
    void drawPage1(TFT_eSprite &spr, int contentW, int contentH);
    void drawPage2(TFT_eSprite &spr, int contentW, int contentH);
    void drawPage3(TFT_eSprite &spr, int contentW, int contentH);
    void drawPage4(TFT_eSprite &spr, int contentW, int contentH);
    void drawPage5(TFT_eSprite &spr, int contentW, int contentH);
}
