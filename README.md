# AudioVisualiser (Experimental)

> [!WARNING]
> This repository is **experimental**. Features may be incomplete, and the code is subject to significant changes.

An advanced audio visualizer built for the Raspberry Pi Pico 2 (RP2350) using the PlatformIO ecosystem. This project
performs real-time FFT (Fast Fourier Transform) analysis on audio input and visualizes the results on both a TFT display
and multiplexed NeoPixel LED strips.

## Features

- **Real-time FFT Analysis**: Processes audio samples to extract frequency magnitude data.
- **TFT Display Integration**: Uses the `TFT_eSPI` library to display frequency bars, peak traces, and a settings UI.
- **Multiplexed LED Strips**: Controls multiple NeoPixel strips using a multiplexed hardware setup. 
- **Dynamic Animations**: Includes various animation modes like `Normal` and `Centre` for LED visualizations.
- **Settings UI**: On-screen menu for adjusting parameters like brightness and visualization modes (TODO).
- **Auto-Loudness Adjustment**: Automatically adjusts sensitivity based on sound levels.

## Hardware & Environment

- **Microcontroller**: Raspberry Pi Pico 2 (RP2350).
- **Display**: ILI9341-based TFT (or similar supported by `TFT_eSPI`).
- **Audio Input**: I2S Microphone.
- **LEDs**: WS2812B (NeoPixel) strips.
- **Framework**: Arduino (Earle Philhower's RP2040/RP2350 core).
- **Build Tool**: PlatformIO.

## Library Dependencies

- `TFT_eSPI`
- `Adafruit NeoPixel`
- `arduino-audio-tools` (RealFFT)

## Project Structure

- `src/`: Main application logic.
- `lib/`: Custom libraries for analysis, animations, display handling, and peripheral control.
- `include/`: Global definitions and constants.
- `other/recs/`: Sample audio recordings for testing/analysis.
