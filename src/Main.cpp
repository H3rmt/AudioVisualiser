#include <Analyze.hpp>
#include <Arduino.h>
#include <arduinoFFT.h>

#include <Core.hpp>
#include <Debug.hpp>
#include <Display.hpp>
#include <Mic.hpp>
#include <Timing.hpp>

#include "Light.h"
#include "MultiplexedStrip.hpp"

Display::Display display;
Settings settings;

volatile bool started = false;

volatile int8_t readyFrameIndex = -1;

namespace counter {
    uint32_t updateBarInfoMillis = millis();

    /// Number of Display refreshes (reset every second)
    uint16_t DisplayRefreshCount = 0;

    /// Number of FFTs (reset every second)
    uint16_t FFTCount = 0;

    /// Number of MicDatas (reset every second)
    uint16_t MicDataCount = 0;

    // Number of LED updates per second
    uint16_t LEDSUpdates = 0;

    void incDisplayRefreshCount() {
        DisplayRefreshCount++;
    }

    void incFFTCount() {
        FFTCount++;
    }

    void incMicDataCount() {
        MicDataCount++;
    }

    void incLEDUpdates() {
        LEDSUpdates++;
    }

    void reset() {
        DisplayRefreshCount = 0;
        FFTCount = 0;
        MicDataCount = 0;
        LEDSUpdates = 0;
    }

    bool check() {
        if (updateBarInfoMillis + 1000 < millis()) {
            updateBarInfoMillis = millis();
            return true;
        }
        return false;
    }
}


namespace audio {
    constexpr uint8_t audioFrameCount = 2;
    Frame audioFrames[audioFrameCount];
    AnalyzedData data[audioFrameCount];

    float vReal[Consts::Samples];
    float vImag[Consts::Samples];
    auto FFT = ArduinoFFT<float>(vReal, vImag, Consts::Samples, Consts::SamplingFrequency, false);

    void fft() {
        memset(vImag, 0, sizeof(vImag));
        Timing::start(Timing::Id::FFT);
        FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
        FFT.compute(FFTDirection::Forward);
        FFT.complexToMagnitude();
        Timing::stop(Timing::Id::FFT);
    }
}

void drawDisplay(const AnalyzedData *data, const AnalyzeDataDynamic *dynamic, const uint8_t frameIndex) {
    if (display.isSettingsMode()) {
        display.dmaWait();
        display.drawSettings();
        display.dmaWrite(true);
    } else {
        display.dmaWait();
        display.drawMain(data, dynamic);
        display.drawRawAudio(audio::audioFrames[frameIndex], dynamic->loudnessDivider, dynamic->off);
        display.drawDebugBars(data, dynamic);
        display.dmaWrite(true);
    }
}

void setup() {
    Debug::setupOnboardLeds();
    Debug::testOnboardLeds();
    Debug::progress(0);
    randomSeed(analogRead(26) * micros());

#ifdef CDEBUG
    Serial.begin(115200);
#ifdef WAIT_FOR_SERIAL
    while (!Serial.available()) {
        Debug::testOnboardLeds();
    }
    Serial.read();
#endif
#endif
    Console::println("\n\n");

#ifdef TEST_LEDS
    display.init(true, &settings);
#else
    display.init(false, &settings);
#endif

    Debug::progress(1);
    Console::println("Display setup complete");
    display.addInfoString("display setup finished", true);
    delay(500);

    display.drawBars();
    Debug::progress(2);
    Console::println("Display bars drawn");
    display.addInfoString("display bars drawn");
    delay(200);

    Debug::progress(3);
    Console::println("Analyze data initialized");
    delay(200);

    const uint32_t pio2MicReserve = Debug::reserveFreePioStateMachines(2);
    // const uint32_t pio1LedReserve = Debug::reserveFreePioStateMachines(1);
    Mic::setupMic();
    // Debug::releasePioStateMachines(1, pio1LedReserve);
    Debug::releasePioStateMachines(2, pio2MicReserve);
    Debug::printPioUsage("after mic init");
    Debug::progress(4);
    Console::println("Mic setup called");
    display.addInfoString("mic setup complete");
    delay(500);

    // const uint32_t pio2LedReserve = Debug::reserveFreePioStateMachines(2);
    setupLeds();
    // Debug::releasePioStateMachines(2, pio2LedReserve);
    Debug::progress(5);
    Console::println("LEDs setup complete");
    display.addInfoString("LED setup complete");
    delay(150);

#ifdef TEST_LEDS
    display.addInfoString("testing LED 1/4");
    selectOutput(0);
    one().testShow(0);
    two().testShow(0);
    Console::println("LEDs 1/4 tested");
    delay(150);
    display.addInfoString("testing LED 2/4", true);
    selectOutput(1);
    // leds::one.test(1);
    two().testShow(1);
    Console::println("LEDs 2/4 tested");
    delay(150);
    display.addInfoString("testing LED 3/4", true);
    selectOutput(2);
    one().testShow(2);
    two().testShow(2);
    Console::println("LEDs 3/4 tested");
    delay(150);
    display.addInfoString("testing LED 4/4", true);
    selectOutput(3);
    one().testShow(3);
    two().testShow(3);
    Console::println("LEDs 4/4 tested");
    delay(150);
#endif

    display.addInfoString("LED testing finished", true);
    Console::println("LEDs tested");
    Debug::progress(6);
    delay(150);

    // TODO load settings
    Debug::printPioUsage("after all init");
    Debug::progress(7);
    delay(50);

    Timing::setStart(0);
    started = true;
}


uint8_t frameIndex = 0;
AnalyzeDataDynamic dynamic;

void loop() {
    Frame &frame = audio::audioFrames[frameIndex];

    Timing::start(Timing::Id::MicStep);
    for (uint16_t i = 0; i < Consts::SamplesCaptured;) {
        if (float sample = 0; Mic::readSample(sample)) {
            frame.samples[i++] = sample;
        } else {
            frame.samples[i++] = 0;
        }
    }
    Timing::stop(Timing::Id::MicStep);
    counter::incMicDataCount();

    const auto samples = &frame.samples[Consts::SamplesCapturedOffset];
    memcpy(audio::vReal, samples, sizeof(audio::vReal));
    audio::fft();
    counter::incFFTCount();

    audio::data[frameIndex] = Analyze::calculate(&dynamic, audio::vReal);
    Analyze::checkChanges(&dynamic, audio::data[frameIndex].peakPeakFrequencyValue);
    Analyze::analyzeFrequencies(&dynamic, &audio::data[frameIndex]);


    if (dynamic.off) {
        counter::incLEDUpdates();
        Timing::start(Timing::Id::DrawLedsOff);
        // one().offAnimation(0);
        two().offAnimation(0);
        selectOutput(0);
        // one().startShow();
        two().startShow();
        Timing::stop(Timing::Id::DrawLedsOff);
    } else {
        counter::incLEDUpdates();
        const auto level = static_cast<uint16_t>(audio::data[frameIndex].peakPeakValue * UINT16_MAX / dynamic.floatingAverage);
        const uint16_t offset = millis() * 12 % UINT16_MAX;
        Console::printf("leds: %f, %f\n\r", level, offset);

        Timing::start(Timing::Id::DrawLeds);
        // if (ledIndex == 0) {
        selectOutput(0);
        renderStrip(one(), 0, settings.frontCentre, level, offset, 20.0f, 15, 3, 1.3f, true);
        // renderStrip(one(), 0, settings.frontCentre, level, offset, 5.0f, 15, 3, 1.3f, true);
        renderStrip(two(), 0, settings.leftFrontBack, level, offset, 2.0f, 15, 2, 1.0f, true);
        // one().startShow();
        two().startShow();
        sleep_us(100);
        // } else if (ledIndex == 1) {
        selectOutput(1);
        renderStrip(two(), 1, settings.rightFrontBack, level, offset, 2.0f, 15, 2, 1.0f, true);
        two().startShow();
        sleep_us(100);

        // } else if (ledIndex == 2) {
        selectOutput(2);
        renderStrip(one(), 2, settings.frontLeft, level, offset, 20.0f, 8, 3, 1.3f, true);
        // renderStrip(one(), 2, settings.frontLeft, level, offset, 5.0f, 8, 3, 1.3f, true);
        renderStrip(two(), 2, settings.rightMiddle, level, offset, 1.0f, 15, 2, 1.0f, true);
        // one().startShow();
        two().startShow();
        sleep_us(100);
        // } else {
        selectOutput(3);
        renderStrip(one(), 3, settings.frontRight, level, offset, 20.0f, 8, 3, 1.3f, true);
        // renderStrip(one(), 3, settings.frontRight, level, offset, 5.0f, 8, 3, 1.3f, true);
        renderStrip(two(), 3, settings.leftMiddle, level, offset, 1.0f, 15, 2, 1.0f, true);
        // one().startShow();
        two().startShow();
        sleep_us(100);
        // }
        // one.testShow(ledIndex);
        Timing::stop(Timing::Id::DrawLeds);
    }


    // TODO investigate
    __sync_synchronize();
    if (readyFrameIndex == -1) {
        readyFrameIndex = frameIndex;
        __sync_synchronize();
        frameIndex = frameIndex + 1;
        if (frameIndex >= audio::audioFrameCount) {
            frameIndex = 0;
        }
    }
}


void setup1() {
    while (!started) {
        delay(500);
    }
    Console::println("Starting loop1");
    Timing::setStart(1);
}


uint8_t displayIndex = 0;

void loop1() {
    while (readyFrameIndex == -1) {
    }
    __sync_synchronize();
    uint8_t fi = (uint8_t) readyFrameIndex;
    readyFrameIndex = -1;
    __sync_synchronize();

    // clone here to prevent modification
    auto d = dynamic;
    if (!display.dmaBusy()) {
        counter::incDisplayRefreshCount();
        drawDisplay(&audio::data[fi], &d, fi);
    }

    if (counter::check()) {
        display.dmaWait();
        display.updateFPS(
            dynamic.loudnessDivider,
            counter::DisplayRefreshCount,
            counter::LEDSUpdates,
            counter::FFTCount,
            counter::MicDataCount
        );
        counter::reset();
        updateBrightness();
    }
}
