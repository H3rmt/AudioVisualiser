#include <Arduino.h>
#include <Core.hpp>

#include "Analyze.hpp"
#include "AnalyzeDefs.hpp"

unsigned long startDecreaseLoudTriggered = 0;
unsigned long startIncreaseLoudTriggered = 0;


void Analyze::calculate(AnalyzeData *data, const float *magnitudes) {
    data->rawDataMax = 0;
    data->resultMax = 0;
    data->peakFrequencyIndex = 0;
    for (int32_t i = 0; i < Consts::Samples; i++) {
        data->rawDataPointer[i] /= data->loudnessDividerN;
        if (const int32_t t = abs(data->rawDataPointer[i]); t > data->rawDataMax) {
            data->rawDataMax = t;
        }
    }
    for (int i = 0; i < Consts::FrequenciesUsable; i++) {
        // increase small value
        const auto value = static_cast<uint16_t>(
            magnitudes[i] * Consts::FrequenciesUsable * 4 / data->loudnessDividerN);

        // adjust the amplitude of the approxBuffer (increase higher frequencies)
        const auto a = static_cast<double>(i) / 60.0;
        const auto res = static_cast<uint32_t>((0.7 + a * a) * value);
        data->results[i] = res;

        if (res > data->resultMax) {
            data->resultMax = res;
            data->peakFrequencyIndex = i;
        }

        if (res > data->peaks[i])
            data->peaks[i] = res;
        else // fall slowly
            data->peaks[i] = static_cast<uint32_t>(
                static_cast<float>(data->peaks[i]) * 0.987 + static_cast<float>(res) * 0.016);
    }
}


// 4 averages per second for 20 seconds
// millis() % is used
constexpr int size = 4 * 20;
auto averages = new int32_t[size]{};
uint8_t lastIndex = 50;

void Analyze::checkChanges(AnalyzeData *data) {
    const uint8_t sec = millis() / 1000 % 20;
    const uint8_t msecSlot = millis() % 1000 / 250;
    const uint8_t slot = sec * 4 + msecSlot;
    if (slot == lastIndex)
        return;
    lastIndex = slot;
    Console::println(
        String("new slot: ") + slot + "  sec:" + sec + "  msec:" + msecSlot + "; current: " + data->rawDataMax);
    averages[slot] = data->rawDataMax;

    // check decrease average every 10 seconds
    if (slot % (10 * 4) == 0) {
        Console::print("checking decrease: ");
        int32_t avg = 0;
        // summ over last 10 seconds
        for (uint8_t i = 0; i < (4 * 10); i++) {
            avg += averages[(slot - i) % size];
        }
        avg /= (4 * 10);
        Console::println(String(avg));
        if (avg <= Consts::RawDecreaseDivider) {
            data->loudnessDividerN -= 1;
        }
    }
    // check increase every 1s
    if (slot % 4 == 0) {
        Console::print("checking increase: ");
        int32_t avg = 0;
        // sum over last 1 second
        for (uint8_t i = 0; i < 4; i++) {
            avg += averages[(slot - i) % size];
        }
        avg /= 4;
        Console::println(String(avg));
        if (avg > Consts::RawIncreaseDivider) {
            data->loudnessDividerN += 1;
        }
    }
    // check off every 20 seconds
    if (slot == 0) {
        Console::print("checking off: ");
        int32_t avg = 0;
        // sum over last 20 seconds
        for (uint8_t i = 0; i < (4 * 20); i++) {
            avg += averages[(slot - i) % size];
        }
        avg /= (4 * 20);
        Console::println(String(avg));
        if (avg <= Consts::RawMinOff) {
            data->off = true;
        }
    }
    // check on every 2 seconds
    if (slot % (2 * 4) == 0) {
        Console::print("checking on: ");
        int32_t avg = 0;
        // sum over last 20 seconds
        for (uint8_t i = 0; i < (4 * 2); i++) {
            avg += averages[(slot - i) % size];
        }
        avg /= (4 * 2);
        Console::println(String(avg));
        if (avg > Consts::RawMinOff) {
            data->off = false;
        }
    }
}


uint32_t getAverageMinValue(const uint8_t frequency) {
    const float frequency_divider = 0.7f + (
                                        static_cast<float>(frequency) / static_cast<float>(Consts::FrequenciesUsable)
                                    ) * (1.7f - 0.7f);
    return static_cast<uint32_t>(60 * frequency_divider);
}

void Analyze::analyzeFrequencies(AnalyzeData *data) {
    // Move the floating peak frequency index towards the detected peak
    const int diff = data->peakFrequencyIndex - data->peakFrequencyIndexFloat;
    if (diff > 0) {
        data->peakFrequencyIndexFloat += max(1, diff / 5);
        // if (data->peakFrequencyIndexFloat > maxAvgFreq)
        // data->peakFrequencyIndexFloat = maxAvgFreq;
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
    data->floatingAverageMin = getAverageMinValue(data->peakFrequencyIndexLazy);
    data->floatingAverage = max(
        data->floatingAverageMin,
        static_cast<int>(data->floatingAverage * 0.99f + data->peakFrequencyValue * 0.022f)
    );
}
