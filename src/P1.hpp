#pragma once

#include "Structs.hpp"

void p1Setup(Shared *shared, AnalyzeData *liveAnalyzeData,
            AnalyzeData *displayAnalyzeData,
            MicData *liveMicData,
            MicData *displayMicData);
void p1Loop(Shared *shared, AnalyzeData *liveAnalyzeData,
            AnalyzeData *displayAnalyzeData,
            MicData *liveMicData,
            MicData *displayMicData);