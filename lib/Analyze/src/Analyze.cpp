#include <Arduino.h>
#include <Core.hpp>

#include "Analyze.hpp"
#include "AnalyzeDefs.hpp"

unsigned long startDecreaseLoudTriggered = 0;
unsigned long startIncreaseLoudTriggered = 0;


void Analyze::calculate(AnalyzeData *data) {
    for (int32_t i = 0; i < Consts::Samples; i++) {
        if (const int32_t t = abs(data->rawDataPointer[i]); t > data->rawDataMax) {
            data->rawDataMax = t;
        }
    }
    for (int i = 0; i < Consts::SamplesUsable; i++) {
        // adjust the amplitude of the approxBuffer (increase higher frequencies)
        const auto a = static_cast<double>(i) / 50.0;
        data->results[i] = static_cast<uint32_t>((0.7 + a * a) * static_cast<double>(data->results[i]));

        if (data->results[i] > data->resultMax) {
            data->resultMax = data->results[i];
        }
    }
}

int Analyze::checkLoudnessDivider(const AnalyzeData *data) {
    if (data->rawDataMax > Consts::IncreaseDivider) {
        startDecreaseLoudTriggered = 0;
        if (startIncreaseLoudTriggered == 0) {
            startIncreaseLoudTriggered = millis();
        }
        if (startIncreaseLoudTriggered + 1000 < millis()) {
            startIncreaseLoudTriggered = 0;
            if (data->loudnessDividerN < divider_max) {
                return 1;
            }
        }
    } else if (data->rawDataMax < Consts::DecreaseDivider) {
        startIncreaseLoudTriggered = 0;
        if (startDecreaseLoudTriggered == 0) {
            startDecreaseLoudTriggered = millis();
        }
        if (startDecreaseLoudTriggered + (10 * 1000) < millis()) {
            startDecreaseLoudTriggered = 0;
            if (data->loudnessDividerN > divider_min) {
                return -1;
            }
        }
    } else {
        startDecreaseLoudTriggered = 0;
        startIncreaseLoudTriggered = 0;
    }
    return 0;
}

unsigned long startOffTriggered = 0;

bool Analyze::checkOff(const AnalyzeData *data) {
    if (data->rawDataMax < Consts::RawMinForOff) {
        if (startOffTriggered == 0) {
            startOffTriggered = millis();
        }
        if (startOffTriggered + OFF_SECONDS * 1000 < millis()) {
            return true;
        }
    } else {
        if (startOffTriggered != 0) {
            startOffTriggered = 0;
        }
    }
    return false;
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


void Analyze::updatePeaks(AnalyzeData *data) {
    for (int i = 0; i < Consts::SamplesUsable; i++) {
        if (data->results[i] > data->peaks[i])
            data->peaks[i] = data->results[i];
        else // fall slowly
            data->peaks[i] = static_cast<uint32_t>(
                static_cast<float>(data->peaks[i]) * 0.987 + static_cast<float>(data->results[i]) * 0.016);

        if (i < Consts::SamplesUsable / 4) {
            if (data->peaks[i] > data->peaks[data->peakFrequencyIndex]) {
                data->peakFrequencyIndex = i;
            }
        }
    }
}

void Analyze::analyzeFrequencies(AnalyzeData *data) {
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
