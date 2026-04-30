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
    constexpr int32_t Samples = 512;
    // In theory yes
    // constexpr int32_t FrequenciesUsable = Samples / 2;
    constexpr int32_t FrequenciesUsable = 134;
    constexpr int32_t SamplingFrequency = 44100;
    constexpr int32_t BitsPerSample = 32;

    constexpr uint32_t RawMinOff = 20;
    constexpr uint32_t RawIncreaseDivider = 70;
    constexpr uint32_t RawDecreaseDivider = 35;
}


/// Data structure for analyzed audio data
struct AnalyzeData {
    /// Points to one of the two buffers with raw audio data
    int16_t *rawDataPointer = nullptr;

    /// Approximate FFT buffer
    uint16_t results[Consts::FrequenciesUsable]{};

    /// Amplitude peak buffer
    uint16_t peaks[Consts::FrequenciesUsable]{};

    /// Maximum value inside the raw Buffer
    int32_t rawDataMax = 0;

    /// Maximum value in the results array
    uint16_t resultMax = 0;

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
    uint16_t peakFrequencyValue = 0;

    /// Value of the current floating average, calculated from floatingAverage and peakFrequency value
    uint16_t floatingAverage = 0;

    /// Minimum value of the floating average, calculated from peakFrequencyIndex and loudnessDivider
    uint16_t floatingAverageMin = 0;
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


enum class LEDMode {
    Normal,
    Centre,
    Circle,
    Off,
};

struct LEDSettings {
    LEDMode mode;
    bool reversed;
    bool rainbow;
    uint8_t brightness;
};

struct Settings {
    LEDSettings frontCentre{
        .mode = LEDMode::Centre,
        .reversed = true,
        .rainbow = true,
        .brightness = 100,
    };
    LEDSettings frontLeft{
        .mode = LEDMode::Circle,
        .reversed = true,
        .rainbow = true,
        .brightness = 100,
    };
    LEDSettings frontRight{
        .mode = LEDMode::Circle,
        .reversed = false,
        .rainbow = true,
        .brightness = 100,
    };
    LEDSettings leftMiddle{
        .mode = LEDMode::Centre,
        .reversed = true,
        .rainbow = true,
        .brightness = 200,
    };
    LEDSettings leftFrontBack{
        .mode = LEDMode::Centre,
        .reversed = true,
        .rainbow = true,
        .brightness = 200,
    };
    LEDSettings rightMiddle{
        .mode = LEDMode::Centre,
        .reversed = true,
        .rainbow = true,
        .brightness = 200,
    };
    LEDSettings rightFrontBack{
        .mode = LEDMode::Centre,
        .reversed = true,
        .rainbow = true,
        .brightness = 200,
    };
};
