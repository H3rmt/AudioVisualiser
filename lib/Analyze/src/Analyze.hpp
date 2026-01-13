#pragma once

#include <Core.hpp>

namespace Analyze {
    /// Analyze the audio stream and fill the AnalyzeData structure
    void analyzeFrequencies(AnalyzeData *data);

    // Check if raw data has been to quiet
    void checkOff(AnalyzeData *data);

    // TODO
    void checkLoudnessDivider(AnalyzeData *data);
} // namespace Analyze
