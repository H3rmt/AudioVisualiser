#include <AudioTools.h>
#include <AudioTools/AudioLibs/AudioRealFFT.h>

#include "Defs.hpp"
#include "Debug.hpp"

// 44100 Hz, Mono, 32 bits per sample
AudioInfo info(SAMPLING_FREQUENCY, 1, 32);
I2SStream i2sStream; // Access I2S as stream
AudioRealFFT fft;    // FFT processor
StreamCopy copier(fft, i2sStream);

void setupMic(void (*callback)(AudioFFTBase &fft))
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
    fcfg.length = SAMPLES;
    fcfg.callback = callback;
    fft.begin(fcfg);

#ifdef CDEBUG
    Serial.println("Mic setup complete");
    Serial.print("FFT size: ");
    Serial.print(fft.size()); // SAMPLES / 2
    Serial.print(", length: ");
    Serial.println(fft.length()); // SAMPLES
#endif
}

void copyMicData()
{
    copier.copy();
}