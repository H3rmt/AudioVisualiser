#pragma once

#include <Core.hpp>

namespace Analyze {
    /// Analyze the audio stream and fill the AnalyzeData structure
    void analyzeFrequencies(AnalyzeData *liveAnalyzeData);
} // namespace Analyze