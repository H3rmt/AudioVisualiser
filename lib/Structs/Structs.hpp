#pragma once

#include <Arduino.h>

#include "Defs.hpp"

struct MicData
{
    // Sample stream buffer with raw loudness values (N samples)
    short streamBuffer[SAMPLES];

    // ApproxFFT sample buffer
    int approxBuffer[SAMPLES];

    // Amount of samples alreay read inside of streamBuffer and approxBuffer
    int sampleCount = 0;
};

struct AnalyzeData
{
    // Amplitude peak buffer
    int peakBuffer[SAMPLES_USABLE];

    // Maximum value in the streamBuffer
    short streamBufferMaxValue = 0;

    // Amount of samples with maximum of all frequencies lower than OFF_THRESHOLD
    uint16_t lessThanOffCounter = 0;

    // No sound is detected over some time
    bool off = false;

    // Divider used for incomming sample values from microphone
    float loudnessDivider = 0.8;

    // Iterations where loudness is less than DECREASE_DIVIDER_PEAK
    uint16_t lessThanLoudnessDividerDecreaseCounter = 0;

    // Index of the peak frequency Average that is adjusted to move towards the detected peak
    uint8_t peakFreaquencyIndexFloat = 0;

    // Index of the peak frequency Average (lazily adjusted)
    uint8_t peakFreaquencyIndexLazy = 0;

    // Index of the peak frequency Average
    uint8_t peakFreaquencyIndex = 0;

    // Value of the peak frequency
    int peakFreaquencyValue = 0;

    // Value of the current floating average, calculated from floatingAverage and peakFreaquency value
    int floatingAverage = 0;

    // Minimum value of the floating average, calculated from peakFreaquencyIndex and loudnessDivider
    int floatingAverageMin = 0;
};

struct Shared
{
    // New Data has been analyzed and is ready for display
    bool newDataForDisplay = false;

    // Display has progressed old data this is is set to true
    bool allowNewDataForDisplay = true;

    // Time it took for one FFT iteration
    uint16_t millisForOneFFT = 0;
};