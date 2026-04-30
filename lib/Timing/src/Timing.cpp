#include "Timing.hpp"

#include <array>
#include <chrono>

#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
#include <pico/time.h>
#include <pico/critical_section.h>
#endif

namespace {
    constexpr std::array<const char *, static_cast<uint8_t>(Timing::Id::Count)> kNames = {
        "AnalyzeCalculate",
        "AnalyzeCheckChanges",
        "AnalyzeFrequencies",
        "DrawLeds",
        "DrawLedsOff",
        "DisplayWait",
        "DisplayMain",
        "SettingsDraw",
        "DisplayRawAudio",
        "DisplayDebugBars",
        "DisplayDebugLines",
        "DisplayHandleTouch",
        "DisplayDmaWait",
        "DisplayDmaWrite",
        "DisplayUpdateFps",
        "MicStep",
    };
}

Timing::System::System() {
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
    critical_section_init(&lock);
#endif
}

Timing::System &Timing::System::instance() {
    static System timing;
    return timing;
}

uint64_t Timing::System::nowNs() const {
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
    return time_us_64() * 1000ULL;
#elif defined(ARDUINO)
    return static_cast<uint64_t>(micros()) * 1000ULL;
#else
    using namespace std::chrono;
    return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
#endif
}

void Timing::System::start(const Id id) {
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
    critical_section_enter_blocking(&lock);
#endif
    auto &entry = entries[static_cast<uint8_t>(id)];
    entry.startNs = nowNs();
    entry.running = true;
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
    critical_section_exit(&lock);
#endif
}

void Timing::System::stop(const Id id) {
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
    critical_section_enter_blocking(&lock);
#endif
    auto &entry = entries[static_cast<uint8_t>(id)];
    if (!entry.running) {
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
        critical_section_exit(&lock);
#endif
        return;
    }
    const uint64_t endNs = nowNs();
    entry.totalNs += endNs - entry.startNs;
    entry.running = false;
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
    critical_section_exit(&lock);
#endif
}

uint64_t Timing::System::timeNs(const Id id) const {
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
    critical_section_enter_blocking(&lock);
#endif
    const auto &entry = entries[static_cast<uint8_t>(id)];
    const uint64_t total = entry.running ? (entry.totalNs + (nowNs() - entry.startNs)) : entry.totalNs;
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350) || defined(PICO_BUILD)
    critical_section_exit(&lock);
#endif
    return total;
}

float Timing::System::timeMs(const Id id) const {
    return static_cast<float>(timeNs(id)) / 1000000.0f;
}

const char *Timing::System::name(const Id id) const {
    return kNames[static_cast<uint8_t>(id)];
}
