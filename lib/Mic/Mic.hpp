#pragma once
#include <AudioTools/AudioLibs/AudioRealFFT.h>

void setupMic(void (*callback)(AudioFFTBase &fft));
void copyMicData();