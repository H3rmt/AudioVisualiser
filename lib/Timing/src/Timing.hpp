#pragma once

#include <Arduino.h>

namespace Timing {
    enum class Id : uint8_t {
        // core 1
        AnalyzeCalculate,
        AnalyzeCheckChanges,
        AnalyzeFrequencies,
        DrawLeds,
        DrawLedsOff,
        MicStep,
        // core 0
        DisplayWait,
        DisplayMain,
        SettingsDraw,
        DisplayRawAudio,
        DisplayDebugBars,
        DisplayDebugLines,
        DisplayHandleTouch,
        DisplayDmaWait,
        DisplayDmaWrite,
        Count, // = number if items in this enum
    };

    class System {
    public:
        static System &instance();

        void start(Id id, uint8_t core = 0);
        void stop(Id id, uint8_t core = 0);

        [[nodiscard]] uint64_t timeNs(Id id, uint8_t core = 0) const;
        [[nodiscard]] float timeMs(Id id, uint8_t core = 0) const;
        [[nodiscard]] const char *name(Id id) const;

        [[nodiscard]] uint64_t nowNs() const;

    private:
        System();

        struct Entry {
            uint64_t totalNs = 0;
            uint64_t startNs = 0;
            bool running = false;
        };

        Entry entries[2][static_cast<uint8_t>(Id::Count)];

#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
        mutable critical_section_t lock{};
#endif
    };

    inline System &get() {
        return System::instance();
    }

    inline void start(const Id id, const uint8_t core = 0) {
        System::instance().start(id, core);
    }

    inline void stop(const Id id, const uint8_t core = 0) {
        System::instance().stop(id, core);
    }

    inline uint64_t timeNs(const Id id, const uint8_t core = 0) {
        return System::instance().timeNs(id, core);
    }

    inline float timeMs(const Id id, const uint8_t core = 0) {
        return System::instance().timeMs(id, core);
    }

    inline const char *name(const Id id) {
        return System::instance().name(id);
    }

    inline uint8_t count() {
        return static_cast<uint8_t>(Id::Count);
    }
}
