#include <Arduino.h>
#include <Core.hpp>

#include "Analyze.hpp"
#include "AnalyzeDefs.hpp"

unsigned long startDecreaseLoudTriggered = 0;

// TODO restrict this to x per second
void Analyze::checkLoudnessDivider(AnalyzeData *data) {
    if (data->rawDataMax > Consts::IncreaseDivider) {
        if (data->loudnessDividerN < divider_max) {
            data->loudnessDividerN += 1;
        }
        if (startDecreaseLoudTriggered != 0) {
            startDecreaseLoudTriggered = 0;
        }
    } else if (data->rawDataMax < Consts::DecreaseDivider) {
        if (startDecreaseLoudTriggered == 0) {
            startDecreaseLoudTriggered = millis();
        }
        if (startDecreaseLoudTriggered + decreaseDividerSeconds * 1000 < millis()) {
            if (data->loudnessDividerN > divider_min) {
                data->loudnessDividerN -= 1;
            }
            startDecreaseLoudTriggered = 0;
        }
    } else {
        if (startDecreaseLoudTriggered != 0) {
            startDecreaseLoudTriggered = 0;
        }
    }
}


unsigned long startOffTriggered = 0;

void Analyze::checkOff(AnalyzeData *data) {
    int32_t streamBufferMaxValue = 0;
    for (int32_t i = 0; i < Consts::Samples; i++) {
        if (const int32_t t = abs(data->rawDataPointer[i]); t > streamBufferMaxValue) {
            streamBufferMaxValue = t;
        }
    }
    data->rawDataMax = streamBufferMaxValue;

    if (streamBufferMaxValue < Consts::RawMinForOff) {
        if (startOffTriggered == 0) {
            startOffTriggered = millis();
        }
        if (startOffTriggered + OFF_SECONDS * 1000 < millis()) {
            data->off = true;
        }
    } else {
        data->off = false;
        if (startOffTriggered != 0) {
            startOffTriggered = 0;
        }
    }
}

uint32_t getAverageMinValue(const uint8_t frequency, const uint16_t loudnessDivider) {
    // Map frequency (0..SAMPLES_USABLE) to a divider between 1.9 (at 0) and 0.5 (at SAMPLES_USABLE)
    const float frequency_divider = 1.9f - ((1.4f * frequency) / Consts::SamplesUsable);
    // Console::print(frequency_divider);
    // Console::print(" ");
    const uint16_t loud_divider = max(8, min(20, map(loudnessDivider, divider_min, divider_max, 6, 30)));
    // Console::println(loud_divider);
    return static_cast<uint32_t>(FLOATING_AVG_MIN_BASE * loud_divider * frequency_divider);
}


void Analyze::analyzeFrequencies(AnalyzeData *data) {
    int peakFreaquencyIndex = 0;
    for (int i = 0; i < Consts::SamplesUsable; i++) {
        if (data->results[i] > data->resultMax) {
            data->resultMax = data->results[i];
        }

        // adjust the amplitude of the approxBuffer (increase higher frequencies)
        // data->results[i] = (0.4 + (i / 14.0)) * data->results[i];
        const auto f = static_cast<double>(i);
        data->results[i] = static_cast<uint32_t>(
            (0.7 + (f / 50.0) * (f / 50.0)) * static_cast<double>(data->results[i]));

        // update the peakBuffer
        if (data->results[i] > data->peaks[i])
            data->peaks[i] = data->results[i];
        else
            data->peaks[i] = data->peaks[i] * 0.987 + data->results[i] * 0.016;

        // Find the peak frequency
        if (i < maxAvgFreq) {
            if (data->peaks[i] > data->peaks[peakFreaquencyIndex]) {
                peakFreaquencyIndex = i;
            }
        }
    }
    data->peakFrequencyIndex = peakFreaquencyIndex;

    // Move the floating peak frequency index towards the detected peak
    const int diff = data->peakFrequencyIndex - data->peakFrequencyIndexFloat;
    if (diff > 0) {
        data->peakFrequencyIndexFloat += max(1, diff / 5);
        if (data->peakFrequencyIndexFloat > maxAvgFreq)
            data->peakFrequencyIndexFloat = maxAvgFreq;
    } else if (diff < 0) {
        const int move = max(1, -diff / 3);
        if (data->peakFrequencyIndexFloat > move)
            data->peakFrequencyIndexFloat -= move;
        else
            data->peakFrequencyIndexFloat = 0;
    }

    // Update the actual peak frequency index only if the floating index is close to the detected peak
    int diff2 = data->peakFrequencyIndex - data->peakFrequencyIndexFloat;
    if (abs(diff2) < 3)
        data->peakFrequencyIndexLazy = data->peakFrequencyIndex;

    data->peakFrequencyValue = data->results[data->peakFrequencyIndexLazy];

    // Update floating average to follow the peak value, with min clamp
    data->floatingAverageMin = getAverageMinValue(data->peakFrequencyIndex, data->loudnessDividerN);
    data->floatingAverage = max(
        data->peakFrequencyValue,
        max(
            data->floatingAverageMin,
            static_cast<int>(data->floatingAverage * 0.99f + data->peakFrequencyValue * 0.022f)));
}
