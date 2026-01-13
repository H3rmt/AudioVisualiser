#pragma once

#include <Arduino.h>

namespace Console {
    void print(const char str[]);

    void print(const String &s);

    void print(int value);

    void print(float value);

    void println(const char str[]);

    void println(const String &s);

    void println(int value);

    void println(float value);

    size_t printf(const char *format, ...);
}

namespace Consts {
    // constexpr int32_t Samples = 256;
    constexpr int32_t Samples = 512;
    // constexpr int32_t Samples = 1024;
    constexpr int32_t SamplesRaw = Samples * 4;
    constexpr int32_t SamplesUsable = Samples / 2;
    constexpr int32_t SamplingFrequency = 44100;

    constexpr uint32_t RawMinForOff = 8000000;
    constexpr uint32_t IncreaseDivider = 35000000;
    constexpr uint32_t DecreaseDivider = 20000000;
}


/// Data structure for analyzed audio data
struct AnalyzeData {
    /// Points to one of the two buffers with raw audio data
    int32_t *rawDataPointer = nullptr;

    /// Approximate FFT buffer
    uint32_t results[Consts::SamplesUsable]{};

    /// Amplitude peak buffer
    uint32_t peaks[Consts::SamplesUsable]{};

    /// Maximum value inside the raw Buffer
    int32_t rawDataMax = 0;

    /// Maximum value in the results array
    uint32_t resultMax = 0;

    /// No sound is detected over some time
    bool off = true;

    /// Divider*10 used for incoming sample values from the Microphone
    uint16_t loudnessDividerN = 8;

    /// Index of the peak frequency Average that is adjusted to move towards the detected peak
    uint8_t peakFrequencyIndexFloat = 0;

    /// Index of the peak frequency Average (lazily adjusted)
    uint8_t peakFrequencyIndexLazy = 0;

    /// Index of the peak frequency Average
    uint8_t peakFrequencyIndex = 0;

    /// Value of the peak frequency
    int peakFrequencyValue = 0;

    /// Value of the current floating average, calculated from floatingAverage and peakFrequency value
    uint32_t floatingAverage = 0;

    /// Minimum value of the floating average, calculated from peakFrequencyIndex and loudnessDivider
    uint32_t floatingAverageMin = 0;
};

/// Program-wide shared data structure
struct Shared {
    /// New Data has been analyzed and is ready for display
    bool newDataForDisplay = false;

    /// If Display has progressed the old data, this is set to true
    bool allowNewDataForDisplay = true;

    /// Number of Display refreshes (reset every second)
    uint16_t DisplayRefreshCount = 0;

    /// Number of FFTs (reset every second)
    uint16_t FFTCount = 0;
};
