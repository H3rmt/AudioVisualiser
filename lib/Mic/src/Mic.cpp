#include <AudioTools.h>
#include <AudioTools/AudioLibs/AudioRealFFT.h>
#include <Core.hpp>

#include "Mic.hpp"

const int Samples = 512;
const int SamplesUsable = 256;
const int SamplingFrequency = 44100;

// 44100 Hz, Mono, 32 bits per sample
AudioInfo info(SamplingFrequency, 1, 32);
I2SStream i2sStream; // Access I2S as stream
AudioRealFFT fft;    // FFT processor
StreamCopy copier(fft, i2sStream);

void Mic::setupMic(void (*callback)(AudioFFTBase &fft))
{
    I2SConfig cfg = i2sStream.defaultConfig(RX_MODE);
    cfg.copyFrom(info);
    cfg.pin_ws = D10;                    // Word Select (LRCLK)
    cfg.pin_bck = D8;                    // Bit Clock (BCLK)
    cfg.pin_data = D9;                   // Data In (DOUT)
    cfg.i2s_format = I2S_PHILIPS_FORMAT; // or try with I2S_LSB_FORMAT
    i2sStream.begin(cfg);

    auto fcfg = fft.defaultConfig(TX_MODE);
    fcfg.copyFrom(info);
    fcfg.window_function_fft = new Hamming();
    fcfg.length = Samples;
    fcfg.callback = callback;
    fft.begin(fcfg);

    Console::println("Mic setup complete");
    Console::print("FFT size: ");
    Console::print(fft.size()); // SAMPLES / 2
    Console::print(", length: ");
    Console::println(fft.length()); // SAMPLES
}

void Mic::runMicStep()
{
    copier.copy();
}
