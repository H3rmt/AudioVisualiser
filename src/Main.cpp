#include <Analyze.hpp>
#include <Arduino.h>
#include <AudioTools/AudioLibs/AudioRealFFT.h>

#ifdef UPLOAD_OTA
#include <WiFi.h>
#include <ArduinoOTA.h>
#endif

#include <Core.hpp>
#include <Debug.hpp>
#include <Display.hpp>
#include <Mic.hpp>

#include "Light.hpp"

Shared globalShared;

// Two analyze data structures for double buffering
AnalyzeData analyzeData1;
AnalyzeData analyzeData2;

AnalyzeData *liveAnalyzeData;
AnalyzeData *displayAnalyzeData;

Display::Display display;

#ifdef UPLOAD_OTA
uint32_t last_ota_time = 0;

void connectWifi() {
    Console::println("Connecting to WiFi: ");
    WiFi.setHostname(ESP_HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        delay(250);
        Console::print(".");
    }
    Console::println("");
    const IPAddress ip = WiFi.localIP();
    Console::printf("IP: %s, hostname: %s\r\n", ip.toString().c_str(), WiFi.getHostname());
    ArduinoOTA.onStart([] {
        Console::println("Start OTA update");
    });
    ArduinoOTA.onProgress([](const unsigned int progress, const unsigned int total) {
        if (millis() - last_ota_time > 500) {
            Console::printf("Progress: %u%%\r\n", progress / (total / 100));
            last_ota_time = millis();
        }
    });
    ArduinoOTA.onError([](const ota_error_t error) {
        Console::printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Console::println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Console::println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Console::println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Console::println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Console::println("End Failed");
        }
    });
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.setHostname(ESP_HOSTNAME);
    ArduinoOTA.begin();
    Console::printf("OTA started with Password %s started\r\n", OTA_PASSWORD);
}
#else
void connectWifi() {
}
#endif

int validFFTs = 0;

void fftResult(AudioFFTBase &fft) {
    if (validFFTs > 200) {
        globalShared.FFTCount++;
        // added * 10 because divider is *10
        // const uint32_t div = 10000 / liveAnalyzeData->loudnessDividerN;
        const uint32_t div = 100000 / liveAnalyzeData->loudnessDividerN;
        for (int i = 0; i < Consts::SamplesUsable; i++) {
            const auto value = static_cast<uint32_t>(fft.magnitude(i) * div);
            liveAnalyzeData->results[i] = value;
        }

        Analyze::checkOff(liveAnalyzeData);
        Analyze::checkLoudnessDivider(liveAnalyzeData);
        Analyze::analyzeFrequencies(liveAnalyzeData);

        if (liveAnalyzeData->off)
            drawLEDsOff();
        else
            drawLEDs(liveAnalyzeData->peakFrequencyValue, liveAnalyzeData->floatingAverage);
    } else {
        validFFTs++;
    }


    // while (!globalShared.allowNewDataForDisplay) {
    // }

    if (globalShared.allowNewDataForDisplay) {
        globalShared.allowNewDataForDisplay = false;

        liveAnalyzeData->rawDataPointer = Mic::getRawValuesBuffer();
        Mic::switchBuffers();

        memcpy(displayAnalyzeData, liveAnalyzeData, sizeof(AnalyzeData));
        globalShared.newDataForDisplay = true;
    }
}

volatile bool started = false;

void setup() {
    Debug::setupOnboardLeds();
    Debug::testOnboardLeds();
    Debug::progress(0);

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
    display.init(true);
#else
    display.init(false);
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

    liveAnalyzeData = &analyzeData1;
    displayAnalyzeData = &analyzeData2;
    Debug::progress(3);
    Console::println("Analyze data initialized");
    delay(200);

    Mic::setupMic(&fftResult);
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
    testLeds(0);
    Console::println("LEDs 1/4 tested");
    delay(150);
    display.addInfoString("testing LED 2/4", true);
    testLeds(1);
    Console::println("LEDs 2/4 tested");
    delay(150);
    display.addInfoString("testing LED 3/4", true);
    testLeds(2);
    Console::println("LEDs 3/4 tested");
    delay(150);
    display.addInfoString("testing LED 4/4", true);
    testLeds(3);
    Console::println("LEDs 4/4 tested");
    delay(150);
#endif

    display.addInfoString("LED testing finished", true);
    Console::println("LEDs tested");
    Debug::progress(6);
    delay(150);

    display.addInfoString("WiFi connecting");
    connectWifi();
    Console::println("WiFi connected");
    display.addInfoString("WiFi connected ", true);
    Debug::progress(7);
    delay(750);
    const auto str = String("Host: ") + ESP_HOSTNAME;
    display.addInfoString(str.c_str());
    delay(250);

    started = true;
}

bool displayUpdate(Shared *shared, const AnalyzeData *data) {
    // wait for next FFT to swap buffers
    if (!shared->newDataForDisplay) {
        return false;
    }
    shared->newDataForDisplay = false;
    shared->allowNewDataForDisplay = false;

    display.dmaWait();
    display.draw(data);
    display.drawRawAudio(data->rawDataPointer, data->off);

    // drawSpriteIndizes(data->peakFrequencyIndexFloat, data->peakFrequencyIndex, data->peakFrequencyIndexLazy);
    // drawSpriteBars(data->results, data->peaks);
    // drawSpriteAudio(spr, data->streamBuffer, data->off); // Doesnt exist any more
    display.drawDebugBars(data);
    display.drawDebugLines(data);
    // Display::dmaWrite();

    display.dmaWrite();
    shared->allowNewDataForDisplay = true;
    return true;
}

// Timer for FPS
unsigned long updateBarInfoMillis = millis();
// Limit FPS to update every 10 ms
unsigned long limitFPSMillis = millis();

void loop() {
    // if (limitFPSMillis + 5 < millis()) {
    // limitFPSMillis = millis();

    if (!displayUpdate(&globalShared, displayAnalyzeData)) {
        return;
    }
    globalShared.DisplayRefreshCount++;
#ifdef UPLOAD_OTA
    ArduinoOTA.handle();
#endif

    if (updateBarInfoMillis + 1000 < millis()) {
        updateBarInfoMillis = millis();
        display.updateFPS(displayAnalyzeData->loudnessDividerN, globalShared.DisplayRefreshCount,
                          globalShared.FFTCount);
        globalShared.DisplayRefreshCount = 0;
        globalShared.FFTCount = 0;
    }
    // }
}

void setup1() {
    while (!started) {
        delay(500);
    }
    Console::println("Starting loop1");
}

void loop1() {
    Mic::runMicStep();
}
