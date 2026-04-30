#include <AudioTools.h>
#include <AudioTools/AudioLibs/AudioRealFFT.h>
#include <I2S.h>

#include <Core.hpp>
#include <Timing.hpp>

#include "Mic.hpp"

AudioInfo info(Consts::SamplingFrequency, 1, Consts::BitsPerSample);

bool secondBuffer = false;
int16_t buf[Consts::Samples] = {};
int16_t bufSecond[Consts::Samples] = {};

bool (*ccallback)(int16_t *, AudioFFTBase &) = nullptr;

BufferedWindow window(new Hann());
AudioRealFFT fft;

auto in = I2S(INPUT);

void Mic::setupMic(bool (*callback)(int16_t *, AudioFFTBase &)) {
    in.setDATA(D6);
    in.setBCLK(D7);
    in.setMCLK(D8);
    // LRCLK = BCLK + 1
    in.setBitsPerSample(Consts::BitsPerSample);
    in.setFrequency(Consts::SamplingFrequency);
    in.setStereo(false);
    in.begin();

    ccallback = callback;
    auto fcfg = fft.defaultConfig(TX_MODE);
    fcfg.copyFrom(info);
    fcfg.window_function_fft = &window;
    fcfg.length = Consts::Samples;
    fft.begin(fcfg);

    memset(buf, 0, Consts::Samples * sizeof(int16_t));
    memset(bufSecond, 0, Consts::Samples * sizeof(int16_t));
}

uint16_t counter = 0;


void Mic::runMicStep() {
    Timing::start(Timing::Id::MicStep, 1);
    int32_t l, r;
    in.read32(&l, &r);
    const int16_t s = l >> 14;
    int16_t *activeBuf = secondBuffer ? bufSecond : buf;
    activeBuf[counter] = s;

    if (counter == Consts::Samples - 1) {
        fft.write(reinterpret_cast<const uint8_t *>(activeBuf), Consts::Samples * sizeof(int16_t));
        Timing::stop(Timing::Id::MicStep, 1);
        const bool switchBuffer = ccallback(activeBuf, fft);
        Timing::start(Timing::Id::MicStep, 1);
        if (switchBuffer)
            secondBuffer = !secondBuffer;
        counter = 0;
    } else {
        counter++;
    }
    Timing::stop(Timing::Id::MicStep, 1);
}
