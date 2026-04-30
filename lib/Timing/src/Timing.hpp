#pragma once

#include <Arduino.h>

namespace Timing {
    enum class Id : uint8_t {
        AnalyzeCalculate,
        AnalyzeCheckChanges,
        AnalyzeFrequencies,
        DrawLeds,
        DrawLedsOff,
        DisplayWait,
        DisplayMain,
        SettingsDraw,
        DisplayRawAudio,
        DisplayDebugBars,
        DisplayDebugLines,
        DisplayHandleTouch,
        DisplayDmaWait,
        DisplayDmaWrite,
        DisplayUpdateFps,
        MicStep,
        Count, // = number if items in this enum
    };

    class System {
    public:
        static System &instance();

        void start(Id id);
        void stop(Id id);

        [[nodiscard]] uint64_t timeNs(Id id) const;
        [[nodiscard]] float timeMs(Id id) const;
        [[nodiscard]] const char *name(Id id) const;

        [[nodiscard]] uint64_t nowNs() const;

    private:
        System();

        struct Entry {
            uint64_t totalNs = 0;
            uint64_t startNs = 0;
            bool running = false;
        };

        Entry entries[static_cast<uint8_t>(Id::Count)];

#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
        mutable critical_section_t lock{};
#endif
    };

    inline System &get() {
        return System::instance();
    }

    inline void start(const Id id) {
        System::instance().start(id);
    }

    inline void stop(const Id id) {
        System::instance().stop(id);
    }

    inline uint64_t timeNs(const Id id) {
        return System::instance().timeNs(id);
    }

    inline float timeMs(const Id id) {
        return System::instance().timeMs(id);
    }

    inline const char *name(const Id id) {
        return System::instance().name(id);
    }
}
