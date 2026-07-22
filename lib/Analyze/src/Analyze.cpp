#include <Arduino.h>
#include <Core.hpp>
#include <Timing.hpp>

#include "Analyze.hpp"

unsigned long startDecreaseLoudTriggered = 0;
unsigned long startIncreaseLoudTriggered = 0;


AnalyzedData Analyze::calculate(AnalyzeDataDynamic *dynamic, const float *magnitudes) {
    Timing::start(Timing::Id::AnalyzeCalculate);
    auto data = AnalyzedData{};
    const float mul2 = 8192.0f / static_cast<float>(dynamic->loudnessDivider);
    for (int i = 0; i < Consts::FrequenciesUsable; i++) {
        const float value = magnitudes[i] * mul2;
        // adjust the amplitude of the approxBuffer (increase higher frequencies)
        const auto a = static_cast<float>(i) / 60.0;
        const auto res = static_cast<float>((0.7 + a * a) * value);
        data.results[i] = res;
        if (res > data.peakPeakValue) {
            data.peakPeakValue = res;
            data.peakPeakIndex = i;
        }

        if (res > dynamic->peaks[i])
            dynamic->peaks[i] = res;
        else // fall slowly
            dynamic->peaks[i] = static_cast<uint16_t>(dynamic->peaks[i] * 0.97 + res * 0.03);

        if (i < Consts::MaxFrequencyIndex && dynamic->peaks[i] > data.peakPeakFrequencyValue) {
            data.peakPeakFrequencyValue = dynamic->peaks[i];
            data.peakPeakFrequencyIndex = i;
        }
    }
    Timing::stop(Timing::Id::AnalyzeCalculate);
    return data;
}


// 4 averages per second for 20 seconds
// millis() % is used
constexpr int size = 4 * 20;
auto averages = new int16_t[size]{};
uint8_t lastIndex = 50;

void Analyze::checkChanges(AnalyzeDataDynamic *data, const float max) {
    Timing::start(Timing::Id::AnalyzeCheckChanges);
    const uint8_t sec = millis() / 1000 % 20;
    const uint8_t msecSlot = millis() % 1000 / 250;
    const uint8_t slot = sec * 4 + msecSlot;
    if (slot == lastIndex) {
        Timing::stop(Timing::Id::AnalyzeCheckChanges);
        return;
    }
    lastIndex = slot;
    Console::println(
        String("new slot: ") + slot + "  sec:" + sec + "  msec:" + msecSlot + "; current: " + max);
    averages[slot] = static_cast<int16_t>(max);

    // check decrease average every 5 seconds, for last 10 seconds
    if (slot % (5 * 4) == 0) {
        Console::print("checking decrease: ");
        int32_t avg = 0;
        // summ over last 10 seconds
        for (uint8_t i = 0; i < 4 * 10; i++) {
            // use positive index arithmetic to avoid negative values when subtracting
            avg += averages[(slot + size - i) % size];
        }
        avg /= (4 * 10);
        Console::println(String(avg));
        if (avg <= Consts::RawDecreaseDivider && data->loudnessDivider > 20) {
            data->loudnessDivider -= 1;
        }
    }
    // check increase every 2s, for last 2 seconds
    if (slot % 8 == 0) {
        Console::print("checking increase: ");
        int32_t avg = 0;
        // sum over last 1 second
        for (uint8_t i = 0; i < 8; i++) {
            // avoid negative index by adding size before modulo
            avg += averages[(slot + size - i) % size];
        }
        avg /= 8;
        Console::println(String(avg));
        if (avg > Consts::RawIncreaseDivider && data->loudnessDivider < 120) {
            data->loudnessDivider += 1;
        }
    }
    // check off every 20 seconds, for last 20 seconds
    if (slot == 0) {
        Console::print("checking off: ");
        int32_t avg = 0;
        // sum over last 20 seconds
        for (uint8_t i = 0; i < (4 * 20); i++) {
            // wrap safely to previous slots
            avg += averages[(slot + size - i) % size];
        }
        avg /= (4 * 20);
        Console::println(String(avg));
        if (avg <= Consts::RawMinOff) {
            data->off = true;
        }
    }
    // check on every 2 seconds, for last 8 seconds
    if (slot % (2 * 4) == 0) {
        Console::print("checking on: ");
        int32_t avg = 0;
        // sum over last 20 seconds
        for (uint8_t i = 0; i < 4 * 8; i++) {
            // wrap safely when subtracting
            avg += averages[(slot + size - i) % size];
        }
        avg /= (4 * 8);
        Console::println(String(avg));
        if (avg > Consts::RawMinOff) {
            data->off = false;
        }
    }
    Timing::stop(Timing::Id::AnalyzeCheckChanges);
}


uint32_t getAverageMinValue(const uint8_t frequency) {
    const float frequency_divider = 1.3f + static_cast<float>(frequency) / static_cast<float>(Consts::FrequenciesUsable)
                                    * (1.7f - 1.3f);
    return static_cast<uint32_t>(70 * frequency_divider);
}

void Analyze::analyzeFrequencies(AnalyzeDataDynamic *analyze, const AnalyzedData *data) {
    Timing::start(Timing::Id::AnalyzeFrequencies);

    // Update floating average to follow the peak value, with min clamp
    // analyze->floatingAverageMin = getAverageMinValue(analyze->peakFrequencyIndexLazy);
    analyze->floatingAverageMin = 130.0f;

    if (const auto pv = data->peakPeakValue * 1.3f; analyze->floatingAverage < pv) {
        analyze->floatingAverage += max(1, (pv - analyze->floatingAverage) / 30);
    } else if (analyze->floatingAverage > pv) {
        analyze->floatingAverage -= max(0.25f, (pv - analyze->floatingAverage) / 80);
    }
    if (const auto pfv = data->peakPeakFrequencyValue * 0.9f; analyze->floatingAverage < pfv) {
        analyze->floatingAverage += max(1, (pfv - analyze->floatingAverage) / 40);
    } else if (analyze->floatingAverage > pfv) {
        analyze->floatingAverage -= max(0.20f, (pfv - analyze->floatingAverage) / 100);
    }
    analyze->floatingAverage = max(analyze->floatingAverageMin, analyze->floatingAverage);

    Timing::stop(Timing::Id::AnalyzeFrequencies);
}
