#pragma once

#include <TFT_eSPI.h>

#include "Core.hpp"


namespace SettingsUI {
    struct Rect {
        int x;
        int y;
        int w;
        int h;
    };

    enum class Action : uint8_t {
        None,
        PageUp,
        PageDown,
        LedModeNormal,
        LedModeCentre,
        LedModeCircle,
        LedModeOff,
        LedReverseToggle,
        LedRainbowToggle,
        LedBrightnessStep,
    };


    class SettingsUI {
    public:
        /// @brief Constructor
        explicit SettingsUI() noexcept = default;

        void init(Settings *settings) {
            this->settings = settings;
        }

        const uint8_t pageCount = 9;
        // Draws the settings UI into the given sprite.
        // pageIndex is clamped to [0, pageCount-1].
        void draw(TFT_eSprite &spr);

        // Handle touches, given sprite-local coordinates.
        // x,y are relative to the sprite (0..width-1, 0..height-1).
        void handleTouch(int x, int y, int spriteW, int spriteH);

        void resetPage() {
            pageIndex = 0;
        }

    private:
        uint8_t pageIndex = 0;

        const int sidebarX = 270;
        const int sidebarW = 50;
        const int buttonH = 48;
        const int pad = 5;
        const int trackW = 10;

        Settings *settings = nullptr;

        void layout(int w, int h, Rect &upBtn, Rect &downBtn, Rect &track) const;

        // Each page draws only the left content area (0..269). Sidebar is drawn elsewhere.
        void drawLEDPage(TFT_eSprite &spr, const String &name, LEDSettings *setting,
                         int contentW, int contentH);

        void drawPage8(TFT_eSprite &spr, int contentW, int contentH);

        void drawPage9(TFT_eSprite &spr, int contentW, int contentH);

        Action actionForTouch(int x, int y, int spriteW, int spriteH);
    };
}
