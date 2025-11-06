#pragma once

// Amount of samples used for FFT
#define SAMPLES 512
// Amount of samples inside buffer after FFT that are usable
#define SAMPLES_USABLE (SAMPLES / 2)

// Frequency range, 44.1kHz sampling frequency
#define SAMPLING_FREQUENCY 44100

// 0 to MAXAVGFREQ Frequencies are used to calculate the peak frequency
#define MAXAVGFREQ (SAMPLES_USABLE / 2)