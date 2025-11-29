#pragma once

#include <Arduino.h>

namespace Console {
    void print(const char str[]);

    void print(const String &s);

    void print(int value);

    void println(const char str[]);

    void println(const String &s);

    void println(int value);

    void printf(const char *format, ...);
}

namespace Consts {
    constexpr int Samples = 512;
    constexpr int SamplesUsable = 256;
    constexpr int SamplingFrequency = 44100;
}


/// Data structure for analyzed audio data
struct AnalyzeData {
    /// Approximate FFT buffer
    int results[Consts::SamplesUsable]{};

    /// Amplitude peak buffer
    int peaks[Consts::SamplesUsable]{};

    /// Maximum value in the streamBuffer
    int resultMax = 0;

    /// Amount of samples with maximum of all frequencies lower than OFF_THRESHOLD (if this exceeds OFF_SECONDS * ESTIMATE_SAMPLES_PER_SECOND, the system is considered "off")
    uint16_t lessThanOffCounter = 0;

    /// No sound is detected over some time
    bool off = false;

    /// Divider used for incoming sample values from the Microphone
    // float loudnessDivider = 0.8;

    /// Iterations where loudness is less than DECREASE_DIVIDER_PEAK
    // uint16_t lessThanLoudnessDividerDecreaseCounter = 0;

    /// Index of the peak frequency Average that is adjusted to move towards the detected peak
    uint8_t peakFrequencyIndexFloat = 0;

    /// Index of the peak frequency Average (lazily adjusted)
    uint8_t peakFrequencyIndexLazy = 0;

    /// Index of the peak frequency Average
    uint8_t peakFrequencyIndex = 0;

    /// Value of the peak frequency
    int peakFrequencyValue = 0;

    /// Value of the current floating average, calculated from floatingAverage and peakFrequency value
    int floatingAverage = 0;

    /// Minimum value of the floating average, calculated from peakFrequencyIndex and loudnessDivider
    int floatingAverageMin = 0;
};

/// Program-wide shared data structure
struct Shared {
    /// New Data has been analyzed and is ready for display
    bool newDataForDisplay = false;

    /// If Display has progressed the old data, this is set to true
    bool allowNewDataForDisplay = true;

    /// Time it took for one FFT iteration
    uint16_t millisForOneFFT = 0;
};
