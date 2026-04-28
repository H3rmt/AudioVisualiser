#include <AudioTools.h>
#include <AudioTools/AudioLibs/AudioRealFFT.h>
#include <Core.hpp>

#include "Mic.hpp"

#include "MemOut.hpp"


// 44100 Hz, Mono, 32 bits per sample
AudioInfo info(Consts::SamplingFrequency, 1, 32);
I2SStream i2sStream;
AudioRealFFT fft;

bool secondBuffer = false;
uint8_t buf[Consts::SamplesRaw] = {};
uint8_t bufSecond[Consts::SamplesRaw] = {};
MemOutput buffer(buf, Consts::SamplesRaw);

StreamCopy copier(buffer, i2sStream);

// BufferedWindow window(new Hamming());
BufferedWindow window(new Rectange());

void Mic::setupMic(void (*callback)(AudioFFTBase &fft)) {
    AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);

    I2SConfig cfg = i2sStream.defaultConfig(RX_MODE);
    cfg.copyFrom(info);
    cfg.pin_ws = D8; // Word Select (LRCLK)
    cfg.pin_bck = D7; // Bit Clock (BCLK)
    cfg.pin_data = D6; // Data In (DOUT)
    cfg.i2s_format = I2S_PHILIPS_FORMAT; // or try with I2S_LSB_FORMAT
    i2sStream.begin(cfg);

    auto fcfg = fft.defaultConfig(TX_MODE);
    fcfg.copyFrom(info);
    fcfg.window_function_fft = new Hann();
    fcfg.length = Consts::Samples;
    fcfg.callback = callback;
    fft.begin(fcfg);

    Console::println("Mic setup complete");
    Console::print("FFT size: ");
    Console::print(fft.size()); // SAMPLES / 2
    Console::print(", length: ");
    Console::println(fft.length()); // SAMPLES

    memset(buf, 0, Consts::SamplesRaw * sizeof(uint8_t));
    memset(bufSecond, 0, Consts::SamplesRaw * sizeof(uint8_t));
}


void Mic::runMicStep() {
    if (!buffer.availableForWrite()) {
        // copier filled buffer, ready for fft
        if (secondBuffer) {
            fft.write(bufSecond, Consts::SamplesRaw);
        } else {
            fft.write(buf, Consts::SamplesRaw);
        }
        buffer.begin();
    }

    copier.copy();
}


void Mic::switchBuffers() {
    secondBuffer = !secondBuffer;
    if (secondBuffer) {
        buffer.setBuffer(bufSecond, Consts::SamplesRaw);
    } else {
        buffer.setBuffer(buf, Consts::SamplesRaw);
    }
}

int32_t *Mic::getRawValuesBuffer() {
    if (secondBuffer) {
        return reinterpret_cast<int32_t *>(bufSecond);
    }
    return reinterpret_cast<int32_t *>(buf);
}
