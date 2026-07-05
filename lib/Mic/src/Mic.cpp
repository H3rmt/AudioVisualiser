#include <I2S.h>

#include <Core.hpp>

#include "Mic.hpp"

auto in = I2S(INPUT);

void Mic::setupMic() {
    in.setDATA(D6);
    in.setBCLK(D7);
    in.setMCLK(D8);
    // LRCLK = BCLK + 1
    in.setBitsPerSample(Consts::BitsPerSample);
    in.setFrequency(Consts::SamplingFrequency);
    in.setStereo(false);
    in.begin();
}

bool Mic::readSample(float &sample) {
    int32_t l, r;
    if (!in.read32(&l, &r)) {
        return false;
    }
    sample = l / 2.147483e9; // 2^31
    // sample = (l >> 14) / 131071.9; // 2^31 >> 14
    return true;
}
