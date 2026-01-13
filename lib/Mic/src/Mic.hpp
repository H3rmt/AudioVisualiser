#pragma once

#include <AudioTools/AudioLibs/AudioRealFFT.h>

namespace Mic
{
    /// Setup the microphone and FFT with given callback
    void setupMic(void (*callback)(AudioFFTBase &fft));

    /// Run one step of microphone processing (move data from I2S to Buffer to FFT)
    /// 
    /// This should be called repeatedly in the main loop
    void runMicStep();

    /// Switch between the 2 internal buffers.
    ///
    /// Should be called from the fft callback so no data is lost.
    void switchBuffers();

    /// Returns a pointer to the raw samples buffer.
    ///
    /// After calling this `switchBuffers` should be used to prevent modifications to this buffer
    int32_t* getRawValuesBuffer();
} // namespace Mic