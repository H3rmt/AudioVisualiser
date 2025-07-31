#include <Arduino.h>

#include "Structs.hpp"
#include "P0.hpp"
#include "P1.hpp"

Shared shared = {
    .newDataForDisplay = false,
    .allowNewDataForDisplay = true,
    .millisForOneFFT = 0,
};

AnalyzeData *liveAnalyzeData;
AnalyzeData *displayAnalyzeData;

MicData *liveMicData;
MicData *displayMicData;


void setup()
{
    p0Setup(&shared, liveAnalyzeData, displayAnalyzeData, liveMicData, displayMicData);
}

void loop()
{
    p0Loop(&shared, liveAnalyzeData, displayAnalyzeData, liveMicData, displayMicData);
}

void setup1()
{
    p1Setup(&shared, liveAnalyzeData, displayAnalyzeData, liveMicData, displayMicData);
}

void loop1()
{
    p1Loop(&shared, liveAnalyzeData, displayAnalyzeData, liveMicData, displayMicData);
}