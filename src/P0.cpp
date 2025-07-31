#include <Arduino.h>
#include <PDM.h>

#include "Analyze.hpp"
#include "Light.hpp"

// Audio sample buffer
short sampleBuffer[128];
// Samples read from microphone (expected to be 64)
int samplesRead = 0;

// Two mic data structures for double buffering
MicData micData1;
MicData micData2;

// Two analyze data structures for double buffering
AnalyzeData analyzeData1;
AnalyzeData analyzeData2;

void p0Setup(
    Shared *shared, AnalyzeData *liveAnalyzeData,
    AnalyzeData *displayAnalyzeData,
    MicData *liveMicData,
    MicData *displayMicData)
{
#ifdef DEBUG
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("START 0");
#endif

    liveAnalyzeData = &analyzeData1;
    displayAnalyzeData = &analyzeData2;
    liveMicData = &micData1;
    displayMicData = &micData2;

    PDM.onReceive([]()
                  {
    int bytesAvailable = PDM.available();
    PDM.read(sampleBuffer, bytesAvailable);
    samplesRead = bytesAvailable / 2; });

    PDM.setGain(20);
    // setup the I2S audio input for the sample rate with 32-bits per sample
    if (!PDM.begin(1, SAMPLING_FREQUENCY)) // 1 channel, 16kHz sample rate
    {
        Serial.println("Failed to start PDM!");
        while (1)
            ;
    }
    Serial.println("PDM started");

    delay(50);
    Serial.println("Setup complete");

    delay(50);
    // initLeds();
    Serial.println("LEDs initialized");
    delay(50);
    testLedsSingle();
}

// Timer for FFT
long readMillis = millis();
void p0Loop(Shared *shared, AnalyzeData *liveAnalyzeData,
            AnalyzeData *displayAnalyzeData,
            MicData *liveMicData,
            MicData *displayMicData)
{
    // Only 64 PDM audio samples are available at a time so accumulate 256 for the FFT
    // new Data from Microphone
    if (samplesRead != 0)
    {
        // This is a super weird bug, arround index 66 and 2 are some weird spikes, idk why
        sampleBuffer[1] = sampleBuffer[0];
        sampleBuffer[2] = sampleBuffer[4];
        sampleBuffer[3] = sampleBuffer[4];

        // Fill the buffers with the samples
        for (int i = 0; i < samplesRead; i++)
        {
            liveMicData->streamBuffer[i + liveMicData->sampleCount] = sampleBuffer[i];
            liveMicData->approxBuffer[i + liveMicData->sampleCount] = sampleBuffer[i] / 2;
        }

        liveMicData->sampleCount += samplesRead;
        samplesRead = 0;

        // enough samples to a FFT iteraton
        if (liveMicData->sampleCount >= SAMPLES)
        {
            shared->millisForOneFFT = millis() - readMillis;
            readMillis = millis();
            liveMicData->sampleCount = 0;
            analyzeData(liveMicData, liveAnalyzeData);

            Serial.print("FFT Iteration took: ");
            Serial.print(shared->millisForOneFFT);
            Serial.println(" ms");

            if (liveAnalyzeData->off)
                drawLEDsOff();
            else
                drawLEDs(liveAnalyzeData->peakFreaquencyValue / 500, liveAnalyzeData->floatingAverage / 500);

            if (shared->allowNewDataForDisplay)
            {
                shared->allowNewDataForDisplay = false;
                AnalyzeData *tmp = liveAnalyzeData;
                liveAnalyzeData = displayAnalyzeData;
                displayAnalyzeData = tmp;
                shared->newDataForDisplay = true;
            }
        }
    }
}
