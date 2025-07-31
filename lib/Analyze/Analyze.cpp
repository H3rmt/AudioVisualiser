#include "Analyze.hpp"

#include <Arduino.h>

#include "ApproxFFT.hpp"
#include "AnalyzeDefs.hpp"

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

int getAverageMinValue(uint8_t frequency, float loudnessDivider)
{
    // Map frequency (0..SAMPLES_USABLE) to a divider between 1.9 (at 0) and 0.5 (at SAMPLES_USABLE)
    float frequency_divider = 1.9f - ((1.4f * frequency) / SAMPLES_USABLE);
    float loud_div = max(0.9f, min(1.8f, map(loudnessDivider, DIVIDER_MIN, DIVIDER_MAX, 0.9f, 3.0f)));
    return FLOATING_AVG_MIN_BASE * loud_div * frequency_divider;
}

void analyzeFrequencies(MicData *liveMicData, AnalyzeData *liveAnalyzeData)
{
    liveAnalyzeData->peakFreaquencyIndex = 0;
    for (int i = 0; i < SAMPLES_USABLE; i++)
    {
        // adjust the amplitude of the approxBuffer (increase higher frequencies)
        liveMicData->approxBuffer[i] = (0.4 + (i / 14.0)) * liveMicData->approxBuffer[i];

        // update the peakBuffer
        if (liveMicData->approxBuffer[i] > liveAnalyzeData->peakBuffer[i])
            liveAnalyzeData->peakBuffer[i] = liveMicData->approxBuffer[i];
        else
            liveAnalyzeData->peakBuffer[i] = liveAnalyzeData->peakBuffer[i] * 0.987 + liveMicData->approxBuffer[i] * 0.016;

        // Find the peak frequency
        if (i < MAXAVGFREQ)
            if (liveAnalyzeData->peakBuffer[i] > liveAnalyzeData->peakBuffer[liveAnalyzeData->peakFreaquencyIndex])
                liveAnalyzeData->peakFreaquencyIndex = i;
    }

    // Move the floating peak frequency index towards the detected peak
    int diff = liveAnalyzeData->peakFreaquencyIndex - liveAnalyzeData->peakFreaquencyIndexFloat;
    if (diff > 0)
    {
        liveAnalyzeData->peakFreaquencyIndexFloat += max(1, diff / 5);
        if (liveAnalyzeData->peakFreaquencyIndexFloat > MAXAVGFREQ)
            liveAnalyzeData->peakFreaquencyIndexFloat = MAXAVGFREQ;
    }
    else if (diff < 0)
    {
        int move = max(1, -diff / 3);
        if (liveAnalyzeData->peakFreaquencyIndexFloat > move)
            liveAnalyzeData->peakFreaquencyIndexFloat -= move;
        else
            liveAnalyzeData->peakFreaquencyIndexFloat = 0;
    }

    // Update the actual peak frequency index only if the floating index is close to the detected peak
    int diff2 = liveAnalyzeData->peakFreaquencyIndex - liveAnalyzeData->peakFreaquencyIndexFloat;
    if (abs(diff2) < 3)
        liveAnalyzeData->peakFreaquencyIndexLazy = liveAnalyzeData->peakFreaquencyIndex;

    liveAnalyzeData->peakFreaquencyValue = liveMicData->approxBuffer[liveAnalyzeData->peakFreaquencyIndex];

    // Update floating average to follow the peak value, with min clamp
    liveAnalyzeData->floatingAverageMin = getAverageMinValue(liveAnalyzeData->peakFreaquencyIndex, liveAnalyzeData->loudnessDivider);
    liveAnalyzeData->floatingAverage = max(
        liveAnalyzeData->peakFreaquencyValue,
        max(
            liveAnalyzeData->floatingAverageMin,
            (int)(liveAnalyzeData->floatingAverage * 0.99f + liveAnalyzeData->peakFreaquencyValue * 0.022f)));
}

void analyzeData(MicData *liveMicData, AnalyzeData *liveAnalyzeData)
{
    // adjust values by loudness divider
    for (int i = 0; i < SAMPLES; i++)
    {
        liveMicData->approxBuffer[i] = liveMicData->approxBuffer[i] / liveAnalyzeData->loudnessDivider;
        liveMicData->streamBuffer[i] = liveMicData->streamBuffer[i] / liveAnalyzeData->loudnessDivider;
    }

    approx_FFT(liveMicData->approxBuffer, SAMPLES, SAMPLING_FREQUENCY);

    liveAnalyzeData->streamBufferMaxValue = 0;
    for (int i = 0; i < SAMPLES; i++)
    {
        if (liveAnalyzeData->streamBufferMaxValue < liveMicData->streamBuffer[i])
            liveAnalyzeData->streamBufferMaxValue = liveMicData->streamBuffer[i];
    }
    checkOff(liveAnalyzeData);
    checkLoudnessDivider(liveAnalyzeData);
    analyzeFrequencies(liveMicData, liveAnalyzeData);
}