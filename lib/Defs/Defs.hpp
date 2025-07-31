#pragma once

// Amount of samples used for FFT
#define SAMPLES (64 * 4)
// Amount of samples inside buffer after FFT that are usable
#define SAMPLES_USABLE (SAMPLES / 4)

// Frequency range, 16kHz sampling frequency
#define SAMPLING_FREQUENCY 16000 

// 0 to MAXAVGFREQ Frequencies are used to calculate the peak frequency
#define MAXAVGFREQ 45 