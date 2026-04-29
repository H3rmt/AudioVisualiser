#pragma once

#include <Core.hpp>

namespace Analyze {
    // populate all data in AnalyzeData
    void calculate(AnalyzeData *data, const float *magnitudes);

    // Check if on/off or loudnessDivider need to change
    void checkChanges(AnalyzeData *data);

    /// Analyze the audio stream and fill the AnalyzeData structure
    void analyzeFrequencies(AnalyzeData *data);
} // namespace Analyze
