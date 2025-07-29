#pragma once

#define SAMPLES (64 * 4)
#define SAMPLES_USABLE (SAMPLES / 4)

#define SAMPLING_FREQUENCY 16000 // Frequency range plotted is 2.5kHz (sampling frequency/4)
#define CHANNELS 1

#define MAXAVGFREQ 45 // Last n frequencies to average for peak frequency
#define FIX_SPIKES
#define FLOATING_AVG_MIN_BASE 90000

#define ESTIMATE_SAMPLES_PER_SECOND 40

#define DIVIDER_MIN 0.7
#define DIVIDER_START 0.8
#define INCREASE_DIVIDER_PEAK 8200
#define DECREASE_DIVIDER_PEAK 4500
#define DECREASE_DIVIDER_SECONDS 10

#define OFF_THRESHOLD 2500
#define OFF_SECONDS 30

// Scale factors for the display
#define TRACE_SCALE 170          // Scale factor for 'scope trace amplitude
#define FFT_SCALE 1100 // Scale factor for FFT bar amplitude 3.5

#define WIDTH_BAR 4
#define WIDTH_TRACE 1

// Display options
#define DRAW_TRACE // Draw a scope type trace
#define DRAW_MAX   // Draw peak frequency bar indicator
#define DRAW_INFO   // Draw info bars bar
// #define DRAW_RED_CROSS // Draw red cross on frequency when values were ignored 

#define DRAW_PEAK  // Draw spectrum peak bar
// #define DOT_PEAK    // Draw spectrum peak dot

// Sprite width and height
#define FULLWIDTH 320
#define SIDEBARWIDTH 50
#define WIDTH (FULLWIDTH - SIDEBARWIDTH - 3) // 4 offset from left
#define FULLHEIGHT 240
#define TOPBARHEIGHT 50
#define HEIGHT (FULLHEIGHT - TOPBARHEIGHT)

#define SUBPIN D6      //   1 PIN
#define LEFTPIN D8    //   1 PIN
#define RIGHTPIN D4    //   1 PIN

#define LEFT2PIN D3    //   2 PIN
#define LEFT3PIN D2    //   3 PIN + 1 PIN

#define RIGHT1PIN D5   //   1 PIN + 3 PIN (D10)
#define RIGHT2PIN D7   //   2 PIN
#define RIGHT3PIN D10   //  3 PIN
// #define RIGHT3PIN D18  // - NOT USED

#define LEFTRIGHT3COUNT 27
#define LEFTRIGHTCOUNT 27
#define SUBCOUNT 59

// #define DEBUG true
// #define DEBUGG true