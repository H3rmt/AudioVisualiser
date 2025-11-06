#include <Arduino.h>
#include <AudioTools/AudioLibs/AudioRealFFT.h>

#include "Defs.hpp"
#include "Analyze.hpp"
#include "Mic.hpp"
#include "Render.hpp"
#include "Debug.hpp"
#include "Light.hpp"
#include "Structs.hpp"

Shared shared = {
    .newDataForDisplay = false,
    .allowNewDataForDisplay = true,
    .millisForOneFFT = 0,
};

// Two analyze data structures for double buffering
AnalyzeData analyzeData1;
AnalyzeData analyzeData2;

AnalyzeData *liveAnalyzeData;
AnalyzeData *displayAnalyzeData;

void displayLoop(void *parameter)
{
    while (1)
    {
        displayUpdate(&shared, displayAnalyzeData);
    }
}

// Timer for FFT
long readMillis = millis();
void fftResult(AudioFFTBase &fft)
{
    shared.millisForOneFFT = millis() - readMillis;
    readMillis = millis();

    float max = 0;
    for (int i = 0; i < SAMPLES_USABLE; i += 10)
    {
        int value = fft.magnitude(i) * 1000000; // TODO  / liveAnalyzeData->loudnessDivider;
        liveAnalyzeData->results[i] = value;
        if (value > max)
        {
            max = value;
            liveAnalyzeData->resultMax = value;
        }
    }

    // checkOff(liveAnalyzeData);
    // checkLoudnessDivider(liveAnalyzeData); // TODO
    analyzeFrequencies(liveAnalyzeData);

    Serial.print(liveAnalyzeData->peakFreaquencyValue);
    Serial.print(" ");
    Serial.println(liveAnalyzeData->floatingAverage);

    // if ((liveAnalyzeData)->off)
    // drawLEDsOff();
    // else
    drawLEDs(liveAnalyzeData->peakFreaquencyValue, liveAnalyzeData->floatingAverage);

    if (shared.allowNewDataForDisplay)
    {
        shared.allowNewDataForDisplay = false;
        AnalyzeData *tmp = liveAnalyzeData;
        liveAnalyzeData = displayAnalyzeData;
        displayAnalyzeData = tmp;
        shared.newDataForDisplay = true;
    }
}

void setup()
{
    setupOnboardLeds();
    progress(1);
#ifdef CDEBUG
    Serial.begin(115200);
    // while (!Serial)
    // ;
    Serial.println("START 0");
#endif
    liveAnalyzeData = &analyzeData1;
    displayAnalyzeData = &analyzeData2;
    progress(2);

    setupMic(&fftResult);
    progress(3);
#ifdef CDEBUG
    Serial.println("Mic setup complete");
#endif

    delay(50);
    setupLeds();
    progress(4);
#ifdef CDEBUG
    Serial.println("LEDs initialized");
#endif

    delay(50);
    testLeds();
    progress(5);
#ifdef CDEBUG
    Serial.println("LEDs tested");
#endif

    delay(50);
    displaySetup();
    progress(6);
#ifdef CDEBUG
    Serial.println("Display setup complete");
#endif


    int stackSize = 2048;
    xTaskCreatePinnedToCore(displayLoop, "DisplayLoop", stackSize, NULL, 1, NULL, 1);
}

void loop()
{
    copyMicData();
}