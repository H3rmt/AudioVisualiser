#include "SettingsUI.hpp"

#include <Arduino.h>

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#include "Util.hpp"

namespace {
    constexpr int sidebarX = 270;
    constexpr int sidebarW = 50;
    constexpr int buttonH = 48;
    constexpr int pad = 5;
    constexpr int trackW = 10;

    struct Rect {
        int x;
        int y;
        int w;
        int h;
    };

    static inline bool contains(const Rect &r, const int px, const int py) {
        return px >= r.x && py >= r.y && px < (r.x + r.w) && py < (r.y + r.h);
    }

    static inline int clampi(const int v, const int lo, const int hi) {
        if (v < lo) return lo;
        if (v > hi) return hi;
        return v;
    }

    static inline float clamp01(float v) {
        if (v < 0.0f) return 0.0f;
        if (v > 1.0f) return 1.0f;
        return v;
    }
}

static void layout(const int w, const int h, Rect &upBtn, Rect &downBtn, Rect &track) {
    const int sbX = sidebarX;
    const int sbW = max(0, min(sidebarW, w - sidebarX));
    const int innerX = sbX + 1;
    const int innerW = max(0, sbW - 2);

    upBtn = Rect{innerX + 3, 3, max(0, innerW - 6), buttonH - 6};
    downBtn = Rect{innerX + 3, h - buttonH + 3, max(0, innerW - 6), buttonH - 6};
    track = Rect{sbX + (sbW - trackW) / 2, buttonH + pad, trackW, max(0, h - (2 * buttonH) - (2 * pad))};
}

void SettingsUI::drawPage1(TFT_eSprite &spr, const int contentW, const int contentH) {
    (void)contentW;
    (void)contentH;
    spr.setTextFont(2);
    spr.setTextColor(rgbTo565(200, 200, 210), rgbTo565(14, 14, 18));
    spr.setCursor(10, 14);
    spr.print("Page 1");
}
void SettingsUI::drawPage2(TFT_eSprite &spr, const int contentW, const int contentH) {
    (void)contentW;
    (void)contentH;
    spr.setTextFont(2);
    spr.setTextColor(rgbTo565(200, 200, 210), rgbTo565(14, 14, 18));
    spr.setCursor(10, 14);
    spr.print("Page 2");
}
void SettingsUI::drawPage3(TFT_eSprite &spr, const int contentW, const int contentH) {
    (void)contentW;
    (void)contentH;
    spr.setTextFont(2);
    spr.setTextColor(rgbTo565(200, 200, 210), rgbTo565(14, 14, 18));
    spr.setCursor(10, 14);
    spr.print("Page 3");
}
void SettingsUI::drawPage4(TFT_eSprite &spr, const int contentW, const int contentH) {
    (void)contentW;
    (void)contentH;
    spr.setTextFont(2);
    spr.setTextColor(rgbTo565(200, 200, 210), rgbTo565(14, 14, 18));
    spr.setCursor(10, 14);
    spr.print("Page 4");
}
void SettingsUI::drawPage5(TFT_eSprite &spr, const int contentW, const int contentH) {
    (void)contentW;
    (void)contentH;
    spr.setTextFont(2);
    spr.setTextColor(rgbTo565(200, 200, 210), rgbTo565(14, 14, 18));
    spr.setCursor(10, 14);
    spr.print("Page 5");
}

SettingsUI::Action SettingsUI::actionForTouch(const int x, const int y, const int spriteW, const int spriteH) {
    Rect upBtn{}, downBtn{}, track{};
    layout(spriteW, spriteH, upBtn, downBtn, track);

    if (contains(upBtn, x, y))
        return Action::PageUp;
    if (contains(downBtn, x, y))
        return Action::PageDown;

    return Action::None;
}

void SettingsUI::draw(TFT_eSprite &spr, int pageIndex, const int pageCount) {
    const int h = spr.height();
    const int w = spr.width();

    const int sbX = sidebarX;
    const int sbW = max(0, min(sidebarW, w - sidebarX));

    const uint16_t bg = rgbTo565(18, 18, 24);
    const uint16_t pageBg = rgbTo565(14, 14, 18);
    const uint16_t border = rgbTo565(80, 80, 90);
    const uint16_t panel = rgbTo565(26, 26, 34);
    const uint16_t track = rgbTo565(40, 40, 54);
    const uint16_t thumb = rgbTo565(180, 180, 190);
    const uint16_t sidebarBg = rgbTo565(12, 12, 16);

    spr.fillSprite(bg);

    // Empty page area (left side).
    spr.fillRect(0, 0, min(sbX, w), h, pageBg);

    // Sidebar (scrollbar) area: single-pixel border to avoid a "double rectangle" look.
    if (sbW > 0) {
        spr.fillRect(sbX, 0, sbW, h, sidebarBg);
        if (sbW >= 2 && h >= 2) {
            spr.fillRect(sbX + 1, 1, sbW - 2, h - 2, panel);
        }
        spr.drawRect(sbX, 0, sbW, h, border);
    }

    // Layout inside sidebar border.
    const int innerX = sbX + 1;
    const int innerW = max(0, sbW - 2);

    // Top button.
    spr.fillRect(innerX + 3, 3, max(0, innerW - 6), buttonH - 6, sidebarBg);
    spr.drawRect(innerX + 3, 3, max(0, innerW - 6), buttonH - 6, border);
    {
        const int cx = sbX + sbW / 2;
        const int cy = buttonH / 2;
        spr.fillTriangle(cx, cy - 7, cx - 8, cy + 6, cx + 8, cy + 6, thumb);
    }

    // Bottom button.
    spr.fillRect(innerX + 3, h - buttonH + 3, max(0, innerW - 6), buttonH - 6, sidebarBg);
    spr.drawRect(innerX + 3, h - buttonH + 3, max(0, innerW - 6), buttonH - 6, border);
    {
        const int cx = sbX + sbW / 2;
        const int cy = h - buttonH / 2;
        spr.fillTriangle(cx, cy + 7, cx - 8, cy - 6, cx + 8, cy - 6, thumb);
    }

    // Scrollbar track.
    Rect upBtn{}, downBtn{}, trackRect{};
    layout(w, h, upBtn, downBtn, trackRect);
    const int trackX = trackRect.x;
    const int trackY = trackRect.y;
    const int trackH = trackRect.h;
    spr.fillRect(trackX, trackY, trackW, trackH, track);
    spr.drawRect(trackX, trackY, trackW, trackH, border);

    // Scrollbar thumb (page indicator).
    const int pc = max(1, pageCount);
    pageIndex = clampi(pageIndex, 0, pc - 1);
    const float scroll01 = (pc <= 1) ? 0.0f : clamp01(static_cast<float>(pageIndex) / static_cast<float>(pc - 1));
    if (trackH > 0) {
        const int thumbH = min(18, max(8, trackH / 6));
        const int thumbTravel = max(0, trackH - thumbH);
        const int thumbY = trackY + static_cast<int>(thumbTravel * scroll01);
        spr.fillRect(trackX + 1, thumbY + 1, trackW - 2, thumbH - 2, thumb);
    }

    // Draw the current page content on the left side (leave empty for now).
    const int contentW = min(sidebarX, w);
    switch (pageIndex) {
        case 0: drawPage1(spr, contentW, h); break;
        case 1: drawPage2(spr, contentW, h); break;
        case 2: drawPage3(spr, contentW, h); break;
        case 3: drawPage4(spr, contentW, h); break;
        default: drawPage5(spr, contentW, h); break;
    }
}
