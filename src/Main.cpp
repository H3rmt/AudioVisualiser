#include <Analyze.hpp>
#include <FreeRTOS.h>
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

QueueHandle_t filledAudioFrameQueue = nullptr;

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
    constexpr uint8_t audioFrameCount = 3;
    Frame audioFrames[audioFrameCount];

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
        // display.dmaWait();
        display.drawSettings();
        display.dmaWrite(true);
    } else {
        // display.dmaWait();
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

    filledAudioFrameQueue = xQueueCreate(1, sizeof(uint8_t));
    if (!filledAudioFrameQueue) {
        Console::println("Queue creation failed");
        Debug::errorExit(4);
    }
    Debug::progress(3);
    Console::println("Analyze data initialized");
    delay(200);

    Mic::setupMic();
    Debug::progress(4);
    Console::println("Mic setup called");
    display.addInfoString("mic setup complete");
    delay(500);

    setupLeds();
    Debug::progress(5);
    Console::println("LEDs setup complete");
    display.addInfoString("LED setup complete");
    delay(150);

#ifdef TEST_LEDS
    display.addInfoString("testing LED 1/4");
    selectOutput(0);
    one()->testShow(0);
    two()->testShow(0);
    Console::println("LEDs 1/4 tested");
    delay(150);
    display.addInfoString("testing LED 2/4", true);
    selectOutput(1);
    // leds::one.test(1);
    two()->testShow(1);
    Console::println("LEDs 2/4 tested");
    delay(150);
    display.addInfoString("testing LED 3/4", true);
    selectOutput(2);
    one()->testShow(2);
    two()->testShow(2);
    Console::println("LEDs 3/4 tested");
    delay(150);
    display.addInfoString("testing LED 4/4", true);
    selectOutput(3);
    one()->testShow(3);
    two()->testShow(3);
    Console::println("LEDs 4/4 tested");
    delay(150);
#endif

    display.addInfoString("LED testing finished", true);
    Console::println("LEDs tested");
    Debug::progress(6);
    delay(150);

    // TODO load settings
    Debug::progress(7);
    delay(50);

    Timing::setStart(0);
    started = true;
}

AnalyzeDataDynamic dynamic;
uint8_t ledIndex = 0;

void loop() {
    uint8_t frameIndex = 0;
    if (xQueueReceive(filledAudioFrameQueue, &frameIndex, portMAX_DELAY) != pdPASS) {
        return;
    }

    Frame &frame = audio::audioFrames[frameIndex];
    // use middle of frame for fft
    const auto samples = &frame.samples[Consts::SamplesCapturedOffset];
    memcpy(audio::vReal, samples, sizeof(audio::vReal));
    audio::fft();
    counter::incFFTCount();

    const auto data = Analyze::calculate(&dynamic, audio::vReal);
    Analyze::checkChanges(&dynamic, data.peakPeakFrequencyValue);
    Analyze::analyzeFrequencies(&dynamic, &data);

    if (!display.dmaBusy()) {
        counter::incDisplayRefreshCount();
        drawDisplay(&data, &dynamic, frameIndex);
    }

    // leds::one.offAnimation(ledIndex);
    // leds::two.offAnimation(ledIndex);
    // leds::selectOutput(0);
    // leds::one.testShow2(0);
    // leds::one.startShow(ledIndex);
    // leds::two.waitShow();
    // leds::two.startShow(ledIndex);

    // ledIndex++;
    // counter::incLEDUpdates();
    // if (ledIndex >= 4) {
    // ledIndex = 0;
    // }
    // ledIndex = 0;

    if (counter::check()) {
        display.updateFPS(
            dynamic.loudnessDivider,
            counter::DisplayRefreshCount,
            counter::LEDSUpdates,
            counter::FFTCount,
            counter::MicDataCount
        );
        counter::reset();
        // updateBrightness();
    }
}

/*

// if (dynamic.off) {
    if (true) {
        leds::selectOutput(0);
        // leds::one.clear();
        leds::one.a(2);
        delay(10);
        // Timing::start(Timing::Id::DrawLedsOff);
        // leds::one.offAnimation(ledIndex);
        // leds::selectOutput(ledIndex);
        // leds::one.testShow(ledIndex);
        // leds::two.offAnimation(ledIndex);
        // leds::one.waitShow();
        // leds::one.startShow(ledIndex);
        // leds::two.waitShow();
        // leds::two.startShow();
        // Timing::stop(Timing::Id::DrawLedsOff);
    } else {
        // const auto level = static_cast<uint16_t>(data.peakPeakValue * UINT16_MAX / dynamic.floatingAverage);
        const auto level = 12;
        const uint16_t offset = millis() * 12 % UINT16_MAX;
        Console::printf("leds: %f, %f\n\r", level, offset);

        // Timing::start(Timing::Id::DrawLeds);
        if (ledIndex == 0) {
            leds::renderStrip(leds::one, 0, settings.frontCentre, level, offset, 20.0f, 15, 3, 1.3f, true);
            leds::renderStrip(leds::two, 0, settings.leftFrontBack, level, offset, 5.0f, 15, 2, 1.0f, true);
        } else if (ledIndex == 1) {
            leds::renderStrip(leds::two, 1, settings.rightFrontBack, level, offset, 5.0f, 15, 2, 1.0f, true);
        } else if (ledIndex == 2) {
            leds::renderStrip(leds::one, 2, settings.frontLeft, level, offset, 20.0f, 8, 3, 1.3f, true);
            leds::renderStrip(leds::two, 2, settings.rightMiddle, level, offset, 5.0f, 15, 2, 1.0f, true);
        } else {
            leds::renderStrip(leds::one, 3, settings.frontRight, level, offset, 20.0f, 8, 3, 1.3f, true);
            leds::renderStrip(leds::two, 3, settings.leftMiddle, level, offset, 5.0f, 15, 2, 1.0f, true);
        }
        leds::one.testShow(ledIndex);
        leds::selectOutput(ledIndex);
        leds::one.startShow(ledIndex);
        leds::two.startShow(ledIndex);
        // Timing::stop(Timing::Id::DrawLeds);
    }
*/

void setup1() {
    while (!started) {
        delay(500);
    }
    Console::println("Starting loop1");
    Timing::setStart(1);
}

uint8_t frameIndex = 0;

void loop1() {
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
    if (xQueueSendToBack(filledAudioFrameQueue, &frameIndex, 0) == pdPASS) {
        // send successfully
        frameIndex = frameIndex + 1;
        if (frameIndex >= audio::audioFrameCount) {
            frameIndex = 0;
        }
    } else {
        // if (one()->canShow()) {
        // one()->offAnimation(0);
        // one()->startShow(0);
        // }
    }
}
