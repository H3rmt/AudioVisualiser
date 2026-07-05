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

    /// @brief Print claimed PIO state machines and instruction memory snapshots.
    void printPioUsage(const char *label = nullptr);

    /// @brief Temporarily claim all currently-free state machines on one PIO.
    uint32_t reserveFreePioStateMachines(uint pioIndex);

    /// @brief Release state machines claimed by reserveFreePioStateMachines().
    void releasePioStateMachines(uint pioIndex, uint32_t mask);

    /// @brief Loops forever
    [[noreturn]] void loop();
} // namespace Debug
