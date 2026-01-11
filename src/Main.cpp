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

Shared globalShared = {
    .newDataForDisplay = false,
    .allowNewDataForDisplay = true,
    .millisForOneFFT = 0,
};

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
    WiFiClass::setHostname(ESP_HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        delay(250);
        Console::print(".");
    }
    Console::println("");
    const IPAddress ip = WiFi.localIP();
    Console::printf("IP: %s, hostname: %s\r\n", ip.toString().c_str(), WiFiClass::getHostname());
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
    ArduinoOTA.setMdnsEnabled(true);
    ArduinoOTA.begin();
    Console::printf("OTA started with Password %s started\r\n", OTA_PASSWORD);
}
#else
void connectWifi() {
}
#endif


void displayUpdate(Shared *shared, AnalyzeData *data) {
    // wait for next FFT to swap buffers
    if (!shared->newDataForDisplay) {
        return;
    }
    shared->newDataForDisplay = false;
    shared->allowNewDataForDisplay = false;

    // Display::dmaWait();
    // drawSpriteIndizes(data->peakFrequencyIndexFloat, data->peakFrequencyIndex, data->peakFrequencyIndexLazy);
    // drawSpriteBars(data->results, data->peaks);
    // drawSpriteAudio(spr, data->streamBuffer, data->off); // Doesnt exist any more
    // drawDebugBars(data, data->results);
    // Display::dmaWrite();

    shared->allowNewDataForDisplay = true;
}

// Timer for FPS
unsigned long startMillis = millis();
// Limit FPS to update every 10 ms
unsigned long limitFPSMillis = millis();
unsigned int counter = 0;

[[noreturn]] void displayLoop(void *parameter) {
    while (true) {
        if (limitFPSMillis + 10 < millis()) {
            limitFPSMillis = millis();
            counter++;

#ifdef UPLOAD_OTA
            ArduinoOTA.handle();
#endif

            // TODO remove
            display.draw(displayAnalyzeData);

            if (startMillis + 1000 < millis()) {
                startMillis = millis();
                // display.updateFPS(displayAnalyzeData->loudnessDivider, fps, 1000.0 / shared->millisForOneFFT);
                display.updateFPS(1.0, counter, 20);
                counter = 0;
            }
            // Console::println("update display");
        }
    }
}

// Timer for FFT
unsigned long readMillis = millis();

void fftResult(AudioFFTBase &fft) {
    globalShared.millisForOneFFT = millis() - readMillis;
    readMillis = millis();

    for (int i = 0; i < Consts::SamplesUsable; i += 10) {
        const int value = static_cast<int>(fft.magnitude(i) * 1000000); // TODO  / liveAnalyzeData->loudnessDivider;
        liveAnalyzeData->results[i] = value;
    }

    // checkOff(liveAnalyzeData);
    // checkLoudnessDivider(liveAnalyzeData); // TODO
    Analyze::analyzeFrequencies(liveAnalyzeData);

    Serial.print(liveAnalyzeData->peakFrequencyValue);
    Serial.print(" ");
    Serial.println(liveAnalyzeData->floatingAverage);

    // if ((liveAnalyzeData)->off)analyzeFrequencies
    // drawLEDsOff();
    // else
    drawLEDs(liveAnalyzeData->peakFrequencyValue, liveAnalyzeData->floatingAverage);

    if (globalShared.allowNewDataForDisplay) {
        globalShared.allowNewDataForDisplay = false;
        AnalyzeData *tmp = liveAnalyzeData;
        liveAnalyzeData = displayAnalyzeData;
        displayAnalyzeData = tmp;
        globalShared.newDataForDisplay = true;
    }
}

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
    Console::printf("Serial initialized, running on core: %d\r\n", xPortGetCoreID());

    display.init(true);
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
    display.addInfoString(str.c_str(), true);
    delay(250);

    constexpr int stackSize = 10000;
    xTaskCreatePinnedToCore(displayLoop, "DisplayLoop", stackSize, nullptr, 0, nullptr, 0);
}

void loop() {
    Mic::runMicStep();
}
