#pragma once

#include "Structs.hpp"

void p0Setup(
    Shared *shared, AnalyzeData *liveAnalyzeData,
    AnalyzeData *displayAnalyzeData,
    MicData *liveMicData,
    MicData *displayMicData);
void p0Loop(Shared *shared, AnalyzeData *liveAnalyzeData,
            AnalyzeData *displayAnalyzeData,
            MicData *liveMicData,
            MicData *displayMicData);