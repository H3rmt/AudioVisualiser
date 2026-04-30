#include "SettingsUI.hpp"

#include <Arduino.h>

#include "Core.hpp"
#include "Util.hpp"

#include <Timing.hpp>

namespace {
    constexpr int kBoxPadding = 6;
    constexpr int kBoxInnerPadding = 6;

    struct LedPageLayout {
        SettingsUI::Rect modePanel{};
        SettingsUI::Rect optionPanel{};
        SettingsUI::Rect modeButtons[4]{};
        SettingsUI::Rect optionButtons[3]{};
    };

    static inline bool contains(const SettingsUI::Rect &r, const int px, const int py) {
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

    static inline LEDSettings *settingForPage(Settings *settings, const uint8_t pageIndex) {
        if (settings == nullptr) return nullptr;
        if (pageIndex == 0) return &settings->frontCentre;
        if (pageIndex == 1) return &settings->frontLeft;
        if (pageIndex == 2) return &settings->frontRight;
        if (pageIndex == 3) return &settings->leftMiddle;
        if (pageIndex == 4) return &settings->leftFrontBack;
        if (pageIndex == 5) return &settings->rightMiddle;
        if (pageIndex == 6) return &settings->rightFrontBack;
        return nullptr;
    }

    static inline LedPageLayout ledPageLayout(const int contentW, const int contentH) {
        LedPageLayout layout{};

        const int pageWidth = max(0, contentW);
        const int pageHeight = max(0, contentH);
        const int innerWidth = max(0, pageWidth - (kBoxPadding * 2));

        const int modePanelY = 45;
        const int optionPanelY = (pageHeight / 2) + 20;
        const int modePanelHeight = max(0, optionPanelY - modePanelY - (kBoxPadding * 2));
        const int optionPanelHeight = max(0, pageHeight - optionPanelY - (kBoxPadding * 2));

        const int modeRowY = modePanelY + kBoxInnerPadding;
        const int modeRowHeight = max(0, modePanelHeight - (kBoxInnerPadding * 2));
        const int modeUsableWidth = max(0, innerWidth - (kBoxInnerPadding * 2));
        const int modeButtonWidth = max(0, min(modeUsableWidth / 4, max(48, modeRowHeight + 14)));
        const int modeButtonGap = (modeButtonWidth > 0 && modeUsableWidth > modeButtonWidth * 4)
                                      ? (modeUsableWidth - (modeButtonWidth * 4)) / 3
                                      : 0;

        const int optionRowY = optionPanelY + kBoxInnerPadding;
        const int optionRowHeight = max(0, optionPanelHeight - (kBoxInnerPadding * 2));
        const int optionUsableWidth = max(0, innerWidth - (kBoxInnerPadding * 2));
        const int optionButtonWidth = max(0, min(optionUsableWidth / 3, max(70, optionRowHeight + 20)));
        const int optionButtonGap = (optionButtonWidth > 0 && optionUsableWidth > optionButtonWidth * 3)
                                        ? (optionUsableWidth - (optionButtonWidth * 3)) / 2
                                        : 0;

        layout.modePanel = SettingsUI::Rect{
            kBoxPadding, modePanelY, max(0, pageWidth - (kBoxPadding * 2)), modePanelHeight
        };
        layout.optionPanel = SettingsUI::Rect{
            kBoxPadding, optionPanelY, max(0, pageWidth - (kBoxPadding * 2)), optionPanelHeight
        };

        for (int i = 0; i < 4; ++i) {
            layout.modeButtons[i] = SettingsUI::Rect{
                kBoxPadding + kBoxInnerPadding + (i * (modeButtonWidth + modeButtonGap)),
                modeRowY,
                modeButtonWidth,
                modeRowHeight,
            };
        }

        for (int i = 0; i < 3; ++i) {
            layout.optionButtons[i] = SettingsUI::Rect{
                kBoxPadding + kBoxInnerPadding + (i * (optionButtonWidth + optionButtonGap)),
                optionRowY,
                optionButtonWidth,
                optionRowHeight,
            };
        }

        return layout;
    }
}

void SettingsUI::SettingsUI::layout(const int w, const int h, Rect &upBtn, Rect &downBtn, Rect &track) const {
    const int sbX = sidebarX;
    const int sbW = max(0, min(sidebarW, w - sidebarX));
    const int innerX = sbX + 1;
    const int innerW = max(0, sbW - 2);

    upBtn = Rect{innerX + 3, 3, max(0, innerW - 6), buttonH - 6};
    downBtn = Rect{innerX + 3, h - buttonH + 3, max(0, innerW - 6), buttonH - 6};
    track = Rect{sbX + (sbW - trackW) / 2, buttonH + pad, trackW, max(0, h - (2 * buttonH) - (2 * pad))};
}

void SettingsUI::SettingsUI::drawLEDPage(TFT_eSprite &spr, const String &name, LEDSettings *setting,
                                         const int contentW, const int contentH) {
    const uint16_t bg = rgbTo565(14, 14, 18);
    const uint16_t border = rgbTo565(80, 80, 90);
    const uint16_t panel = rgbTo565(26, 26, 34);
    const uint16_t primary = rgbTo565(55, 125, 235);
    const uint16_t primaryDark = rgbTo565(22, 58, 120);
    const uint16_t text = rgbTo565(200, 200, 210);

    const int x0 = 0;
    const int y0 = 0;
    const int w = max(0, contentW);
    const int h = max(0, contentH);

    spr.fillRect(x0, y0, w, h, bg);

    spr.setTextFont(4);
    spr.setTextColor(text, bg);
    spr.setCursor(8, 12);
    spr.print("LED: ");
    spr.print(name);

    spr.setTextFont(2);

    if (setting == nullptr || w <= 0 || h <= 0) return;

    const LedPageLayout layout = ledPageLayout(w, h);

    auto drawButton = [&](const Rect &r, const String &line1, const String &line2 = String(),
                          const bool selected = false) {
        const uint16_t fill = selected ? primary : panel;
        const uint16_t fillEdge = selected ? primaryDark : panel;
        spr.fillRoundRect(r.x, r.y, r.w, r.h, 8, fill);
        spr.drawRoundRect(r.x, r.y, r.w, r.h, 8, selected ? primary : border);
        if (!selected) {
            spr.drawRoundRect(r.x + 1, r.y + 1, max(0, r.w - 2), max(0, r.h - 2), 7, fillEdge);
        }
        spr.setTextColor(text, fill);
        spr.setTextDatum(MC_DATUM);
        const int cx = r.x + r.w / 2;
        const int cy = r.y + r.h / 2;
        if (line2.length() > 0) {
            spr.drawString(line1, cx, cy - 10);
            spr.drawString(line2, cx, cy + 8);
        } else {
            spr.drawString(line1, cx, cy);
        }
        spr.setTextDatum(TL_DATUM);
    };

    // Top half: LED mode selection.
    spr.fillRoundRect(layout.modePanel.x, layout.modePanel.y, layout.modePanel.w, layout.modePanel.h, 10, panel);
    spr.drawRoundRect(layout.modePanel.x, layout.modePanel.y, layout.modePanel.w, layout.modePanel.h, 10, border);
    drawButton(layout.modeButtons[0], "Normal", String(), setting->mode == LEDMode::Normal);
    drawButton(layout.modeButtons[1], "Centre", String(), setting->mode == LEDMode::Centre);
    drawButton(layout.modeButtons[2], "Circle", String(), setting->mode == LEDMode::Circle);
    drawButton(layout.modeButtons[3], "Off", String(), setting->mode == LEDMode::Off);

    // Bottom half: option buttons.
    spr.fillRoundRect(layout.optionPanel.x, layout.optionPanel.y, layout.optionPanel.w, layout.optionPanel.h, 10,
                      panel);
    spr.drawRoundRect(layout.optionPanel.x, layout.optionPanel.y, layout.optionPanel.w, layout.optionPanel.h, 10,
                      border);
    drawButton(layout.optionButtons[0], "Reverse", setting->reversed ? "On" : "Off", setting->reversed);
    drawButton(layout.optionButtons[1], "Rainbow", setting->rainbow ? "On" : "Off", setting->rainbow);
    drawButton(layout.optionButtons[2], "Brightness", String(setting->brightness), false);
}

SettingsUI::Action SettingsUI::SettingsUI::actionForTouch(const int x, const int y, const int spriteW,
                                                          const int spriteH) {
    Rect upBtn{}, downBtn{}, track{};
    this->layout(spriteW, spriteH, upBtn, downBtn, track);

    const LedPageLayout layout = ledPageLayout(min(sidebarX, spriteW), spriteH);

    if (pageIndex < 7) {
        if (contains(layout.modeButtons[0], x, y)) return Action::LedModeNormal;
        if (contains(layout.modeButtons[1], x, y)) return Action::LedModeCentre;
        if (contains(layout.modeButtons[2], x, y)) return Action::LedModeCircle;
        if (contains(layout.modeButtons[3], x, y)) return Action::LedModeOff;
        if (contains(layout.optionButtons[0], x, y)) return Action::LedReverseToggle;
        if (contains(layout.optionButtons[1], x, y)) return Action::LedRainbowToggle;
        if (contains(layout.optionButtons[2], x, y)) return Action::LedBrightnessStep;
    }

    if (contains(upBtn, x, y))
        return Action::PageUp;
    if (contains(downBtn, x, y))
        return Action::PageDown;

    return Action::None;
}

void SettingsUI::SettingsUI::handleTouch(const int x, const int y, const int spriteW, const int spriteH) {
    auto action = actionForTouch(x, y, spriteW, spriteH);
    if (action == Action::PageUp) {
        if (pageIndex > 0) {
            pageIndex--;
            Console::println(String("settings page -> ") + (pageIndex + 1));
        }
    } else if (action == Action::PageDown) {
        if (pageIndex + 1 < pageCount) {
            pageIndex++;
            Console::println(String("settings page -> ") + (pageIndex + 1));
        }
    } else {
        LEDSettings *setting = settingForPage(settings, pageIndex);
        if (setting == nullptr) return;

        switch (action) {
            case Action::LedModeNormal:
                setting->mode = LEDMode::Normal;
                break;
            case Action::LedModeCentre:
                setting->mode = LEDMode::Centre;
                break;
            case Action::LedModeCircle:
                setting->mode = LEDMode::Circle;
                break;
            case Action::LedModeOff:
                setting->mode = LEDMode::Off;
                break;
            case Action::LedReverseToggle:
                setting->reversed = !setting->reversed;
                break;
            case Action::LedRainbowToggle:
                setting->rainbow = !setting->rainbow;
                break;
            case Action::LedBrightnessStep:
                setting->brightness = static_cast<uint8_t>(setting->brightness + 10);
                break;
            default:
                break;
        }
    }
}

void SettingsUI::SettingsUI::draw(TFT_eSprite &spr) {
    Timing::start(Timing::Id::SettingsDraw);
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
    spr.drawRect(innerX + 3, 3, max(0, innerW - 6), buttonH - 6, border); {
        const int cx = sbX + sbW / 2;
        const int cy = buttonH / 2;
        spr.fillTriangle(cx, cy - 7, cx - 8, cy + 6, cx + 8, cy + 6, thumb);
    }

    // Bottom button.
    spr.fillRect(innerX + 3, h - buttonH + 3, max(0, innerW - 6), buttonH - 6, sidebarBg);
    spr.drawRect(innerX + 3, h - buttonH + 3, max(0, innerW - 6), buttonH - 6, border); {
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
        case 0:
            drawLEDPage(spr, "Front Centre", &settings->frontCentre, contentW, h);
            break;
        case 1:
            drawLEDPage(spr, "Front Left", &settings->frontLeft, contentW, h);
            break;
        case 2:
            drawLEDPage(spr, "Front Right", &settings->frontRight, contentW, h);
            break;
        case 3:
            drawLEDPage(spr, "Left Centre", &settings->leftMiddle, contentW, h);
            break;
        case 4:
            drawLEDPage(spr, "Left Front Back", &settings->leftFrontBack, contentW, h);
            break;
        case 5:
            drawLEDPage(spr, "Right Centre", &settings->rightMiddle, contentW, h);
            break;
        case 6:
            drawLEDPage(spr, "Right Front Back", &settings->rightFrontBack, contentW, h);
            break;
        default: drawPage8(spr, contentW, h);
            break;
    }
    Timing::stop(Timing::Id::SettingsDraw);
}

void SettingsUI::SettingsUI::drawPage8(TFT_eSprite &spr, const int contentW, const int contentH) {
    spr.fillRect(0, 0, contentW, contentH, rgbTo565(14, 14, 18));
    spr.setTextFont(2);
    spr.setTextColor(rgbTo565(200, 200, 210), rgbTo565(14, 14, 18));
    spr.drawString("Page 8", 10, 14);
}
