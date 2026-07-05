#pragma once

#include <Core.hpp>

namespace Analyze {
    // populate all data in AnalyzeData
    AnalyzedData calculate(AnalyzeDataDynamic *dynamic, const float *magnitudes);

    // Check if on/off or loudnessDivider need to change
    void checkChanges(AnalyzeDataDynamic *data, float rawDataMax);

    /// Analyze the audio stream and fill the AnalyzeData structure
    void analyzeFrequencies(AnalyzeDataDynamic *analyze, const AnalyzedData *data);
} // namespace Analyze
