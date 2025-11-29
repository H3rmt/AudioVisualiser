#pragma once

namespace Debug {
    /// @brief Setup the onboard leds for debug progress indication
    void setupOnboardLeds();

    /// @brief Test the onboard leds by cycling through them
    void testOnboardLeds();

    /// @brief color led to show progress in code
    ///
    /// 0 = off
    /// 1 = red
    /// 2 = green
    /// 3 = yellow
    /// 4 = blue
    /// 5 = purple
    /// 6 = cyan
    /// 7 = white
    /// @param code
    void progress(int code);

    /// @brief Indicate error and halt execution with onboard leds
    [[noreturn]] void errorExit(int code);
} // namespace Debug
