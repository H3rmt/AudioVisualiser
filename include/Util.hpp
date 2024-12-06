#pragma once
#include <Arduino.h>
#include <PDM.h>

// Define a typedef for the function pointer
typedef void (*FetchFunction)(short sampleBuffer[128], volatile int *samplesRead);

uint16_t rainbowColor(uint8_t spectrum);