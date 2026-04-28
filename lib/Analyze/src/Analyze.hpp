#pragma once

#include <Core.hpp>

namespace Analyze {
    // populate all data in AnalyzeData
    void calculate(AnalyzeData *data);

    /// Analyze the audio stream and fill the AnalyzeData structure
    void analyzeFrequencies(AnalyzeData *data);

    // Check if raw data has been to quiet
    bool checkOff(const AnalyzeData *data);

    // update peaks array
    void updatePeaks(AnalyzeData *data);

    // Return change that should be added to the loudness divider
    int checkLoudnessDivider(const AnalyzeData *data);
} // namespace Analyze
