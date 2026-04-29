#pragma once

#include <AudioTools/AudioLibs/AudioRealFFT.h>

namespace Mic {
    /// Setup the microphone and FFT with given callback
    void setupMic(bool (*callback)(int16_t *, AudioFFTBase &));

    // void setupMic(void (*callback)(AudioFFTBase &fft));

    /// Run one step of microphone processing (move data from I2S to Buffer to FFT)
    /// 
    /// This should be called repeatedly in the main loop
    void runMicStep();
} // namespace Mic
