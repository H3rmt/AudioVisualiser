#include <Arduino.h>
#include <Core.hpp>

#include "Analyze.hpp"
#include "AnalyzeDefs.hpp"

#ifdef UIIA
// TODO

void checkOff(AnalyzeData *liveAnalyzeData)
{
    if (liveAnalyzeData->streamBufferMaxValue < OFF_THRESHOLD)
    {
        liveAnalyzeData->lessThanOffCounter++;
        if (liveAnalyzeData->lessThanOffCounter > OFF_SECONDS * ESTIMATE_SAMPLES_PER_SECOND)
        {
            liveAnalyzeData->off = true;
            liveAnalyzeData->lessThanOffCounter = 0;
        }
    }
    else
    {
        liveAnalyzeData->off = false;
        liveAnalyzeData->lessThanOffCounter = 0;
    }
}

void checkLoudnessDivider(AnalyzeData *liveAnalyzeData)
{
    if (liveAnalyzeData->streamBufferMaxValue > INCREASE_DIVIDER_PEAK)
    {
        liveAnalyzeData->loudnessDivider += 0.05;
        liveAnalyzeData->lessThanLoudnessDividerDecreaseCounter = 0;
    }
    else if (liveAnalyzeData->streamBufferMaxValue < DECREASE_DIVIDER_PEAK)
    {
        liveAnalyzeData->lessThanLoudnessDividerDecreaseCounter++;
        if (liveAnalyzeData->lessThanLoudnessDividerDecreaseCounter > DECREASE_DIVIDER_SECONDS * ESTIMATE_SAMPLES_PER_SECOND)
        {
            if (liveAnalyzeData->loudnessDivider > DIVIDER_MIN)
            {
                liveAnalyzeData->loudnessDivider -= 0.1;
            }
            liveAnalyzeData->lessThanLoudnessDividerDecreaseCounter = 0;
        }
    }
    else
    {
        liveAnalyzeData->lessThanLoudnessDividerDecreaseCounter = 0;
    }
}

#endif

constexpr float divider_min = 0.7f;
constexpr float divider_max = 4.0f;

int getAverageMinValue(uint8_t frequency, float loudnessDivider)
{
    // Map frequency (0..SAMPLES_USABLE) to a divider between 1.9 (at 0) and 0.5 (at SAMPLES_USABLE)
    float frequency_divider = 1.9f - ((1.4f * frequency) / Consts::SamplesUsable);
    float aa = map(loudnessDivider, divider_min, divider_max, 0.9f, 3.0f);
    float loud_div = max(0.9f, min(1.8f, aa));
    return FLOATING_AVG_MIN_BASE * loud_div * frequency_divider;
}

constexpr int maxAvgFreq = 45;

void Analyze::analyzeFrequencies(AnalyzeData *data)
{
    int peakFreaquencyIndex = 0;
    for (int i = 0; i < Consts::SamplesUsable; i++)
    {
        if (data->results[i] > data->resultMax) {
            data->resultMax = data->results[i];
        }

        // TODO check if needed
        // adjust the amplitude of the approxBuffer (increase higher frequencies)
        // data->results[i] = (0.4 + (i / 14.0)) * data->results[i];

        // update the peakBuffer
        if (data->results[i] > data->peaks[i])
            data->peaks[i] = data->results[i];
        else
            data->peaks[i] = data->peaks[i] * 0.987 + data->results[i] * 0.016;

        // Find the peak frequency
        if (i < maxAvgFreq)
        {
            if (data->peaks[i] > data->peaks[peakFreaquencyIndex])
            {
                peakFreaquencyIndex = i;
            }
        }
    }
    data->peakFrequencyIndex = peakFreaquencyIndex;

    // Move the floating peak frequency index towards the detected peak
    int diff = data->peakFrequencyIndex - data->peakFrequencyIndexFloat;
    if (diff > 0)
    {
        data->peakFrequencyIndexFloat += max(1, diff / 5);
        if (data->peakFrequencyIndexFloat > maxAvgFreq)
            data->peakFrequencyIndexFloat = maxAvgFreq;
    }
    else if (diff < 0)
    {
        int move = max(1, -diff / 3);
        if (data->peakFrequencyIndexFloat > move)
            data->peakFrequencyIndexFloat -= move;
        else
            data->peakFrequencyIndexFloat = 0;
    }

    // Update the actual peak frequency index only if the floating index is close to the detected peak
    int diff2 = data->peakFrequencyIndex - data->peakFrequencyIndexFloat;
    if (abs(diff2) < 3)
        data->peakFrequencyIndexLazy = data->peakFrequencyIndex;

    data->peakFrequencyValue = data->results[data->peakFrequencyIndex];

    // Update floating average to follow the peak value, with min clamp
    data->floatingAverageMin = getAverageMinValue(data->peakFrequencyIndex, 1.0); // TODO data->loudnessDivider);
    data->floatingAverage = max(
        data->peakFrequencyValue,
        max(
            data->floatingAverageMin,
            (int)(data->floatingAverage * 0.99f + data->peakFrequencyValue * 0.022f)));
}
