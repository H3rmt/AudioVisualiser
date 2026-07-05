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
    constexpr int32_t Samples = 1024;
    constexpr int32_t SamplesCapturedOffset = 256;
    constexpr int32_t SamplesCaptured = 1536;

    constexpr int32_t MaxFrequencyIndex = 90;
    // In theory yes, but we dont have more space on display
    // constexpr int32_t FrequenciesUsable = Samples / 2;
    constexpr int32_t FrequenciesUsable = 134;
    constexpr int32_t SamplingFrequency = 44100;
    constexpr int32_t BitsPerSample = 32;

    // TODO 30 is lower than 90 which is the minimum
    constexpr uint32_t RawMinOff = 30;
    constexpr uint32_t RawIncreaseDivider = 140;
    constexpr uint32_t RawDecreaseDivider = 95;
}


/// Data structure for analyzed audio data
struct AnalyzedData {
    /// Approximate FFT buffer
    float results[Consts::FrequenciesUsable]{};

    /// Value of the peak
    float peakPeakValue = 0;

    /// Index of the biggest peak
    uint8_t peakPeakIndex = 0;

    /// Maximum value in the results array
    float peakPeakFrequencyValue = 0;

    /// Index of the biggest peak frequency
    uint8_t peakPeakFrequencyIndex = 0;
};


struct AnalyzeDataDynamic {
    /// Amplitude peak buffer
    float peaks[Consts::FrequenciesUsable]{};

    /// used for incoming sample values from the Microphone
    uint8_t loudnessDivider = 40;

    /// Value of the current floating average, calculated from floatingAverage and peakFrequency value
    float floatingAverage = 0;

    /// Minimum value of the floating average, calculated from peakFrequencyIndex and loudnessDivider
    float floatingAverageMin = 0;

    /// No sound is detected over some time
    bool off = true;
};

struct Frame {
    float samples[Consts::SamplesCaptured]{};
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
