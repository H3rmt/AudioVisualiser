#include <Arduino.h>

#include "AnalyzeDefs.hpp"
#include "Structs.hpp"

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

int getAverageMinValue(uint8_t frequency, float loudnessDivider)
{
    // Map frequency (0..SAMPLES_USABLE) to a divider between 1.9 (at 0) and 0.5 (at SAMPLES_USABLE)
    float frequency_divider = 1.9f - ((1.4f * frequency) / SAMPLES_USABLE);
    float aa = map(loudnessDivider, DIVIDER_MIN, DIVIDER_MAX, 0.9f, 3.0f);
    float loud_div = max(0.9f, min(1.8f, aa));
    return FLOATING_AVG_MIN_BASE * loud_div * frequency_divider;
}

void analyzeFrequencies(AnalyzeData *data)
{
    int peakFreaquencyIndex = 0;
    for (int i = 0; i < SAMPLES_USABLE; i++)
    {
        // adjust the amplitude of the approxBuffer (increase higher frequencies)
        // data->results[i] = (0.4 + (i / 14.0)) * data->results[i];

        // update the peakBuffer
        if (data->results[i] > data->peaks[i])
            data->peaks[i] = data->results[i];
        else
            data->peaks[i] = data->peaks[i] * 0.987 + data->results[i] * 0.016;

        // Find the peak frequency
        if (i < MAXAVGFREQ)
        {
            if (data->peaks[i] > data->peaks[peakFreaquencyIndex])
            {
                peakFreaquencyIndex = i;
            }
        }
    }
    data->peakFreaquencyIndex = peakFreaquencyIndex;

    // Move the floating peak frequency index towards the detected peak
    int diff = data->peakFreaquencyIndex - data->peakFreaquencyIndexFloat;
    if (diff > 0)
    {
        data->peakFreaquencyIndexFloat += max(1, diff / 5);
        if (data->peakFreaquencyIndexFloat > MAXAVGFREQ)
            data->peakFreaquencyIndexFloat = MAXAVGFREQ;
    }
    else if (diff < 0)
    {
        int move = max(1, -diff / 3);
        if (data->peakFreaquencyIndexFloat > move)
            data->peakFreaquencyIndexFloat -= move;
        else
            data->peakFreaquencyIndexFloat = 0;
    }

    // Update the actual peak frequency index only if the floating index is close to the detected peak
    int diff2 = data->peakFreaquencyIndex - data->peakFreaquencyIndexFloat;
    if (abs(diff2) < 3)
        data->peakFreaquencyIndexLazy = data->peakFreaquencyIndex;

    data->peakFreaquencyValue = data->results[data->peakFreaquencyIndex];

    // Update floating average to follow the peak value, with min clamp
    data->floatingAverageMin = getAverageMinValue(data->peakFreaquencyIndex, 1.0); // TODO data->loudnessDivider);
    data->floatingAverage = max(
        data->peakFreaquencyValue,
        max(
            data->floatingAverageMin,
            (int)(data->floatingAverage * 0.99f + data->peakFreaquencyValue * 0.022f)));
}
