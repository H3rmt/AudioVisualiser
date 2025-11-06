#include <AudioTools.h>

#include "Debug.hpp"

// used to transfer audio data to serial for debugging (using test.py script)
#ifdef DEBUG_AUDIO
// 44100 Hz, Mono, 32 bits per sample
AudioInfo info(44100, 1, 32);
I2SStream i2sStream; // Access I2S as stream

CsvOutput<int32_t> csvOutput(Serial); // ASCII stream

uint8_t *buf = (uint8_t *)malloc(1024 * 1024);
MemoryOutput out(buf, 1024 * 1024); // Memory output

MemoryStream stream(buf, 1024 * 1024);
StreamCopy copier(out, i2sStream);     // copy in to out
StreamCopy copier2(csvOutput, stream); // copy out to csvOutput

void setupMicDebug()
{
    I2SConfig cfg = i2sStream.defaultConfig(RX_MODE);
    cfg.copyFrom(info);
    cfg.pin_ws = D10;                    // Word Select (LRCLK)
    cfg.pin_bck = D8;                    // Bit Clock (BCLK)
    cfg.pin_data = D9;                   // Data In (DOUT)
    cfg.i2s_format = I2S_PHILIPS_FORMAT; // or try with I2S_LSB_FORMAT
    i2sStream.begin(cfg);

    csvOutput.begin(info);
    out.begin();
}

void sendDataChunksLoop()
{
    progress(7);
    while (1)
    {
        if (!out.availableForWrite())
        {
            progress(4);
            stream.begin();
            while (stream.available())
            {
                copier2.copy();
            }
            out.begin(); // reset memory output
            progress(7);
        }
        copier.copy();
    }
}

#endif