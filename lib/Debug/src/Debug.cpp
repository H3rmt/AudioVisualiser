#include <Arduino.h>
#include <Core.hpp>

#include "Debug.hpp"

void Debug::setupOnboardLeds() {
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    // pinMode(LED_BUILTIN, OUTPUT);
    // digitalWrite(LED_BUILTIN, HIGH);
}

void Debug::progress(const int code) {
    if (code == 0) {
        // digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_BLUE, HIGH);
        return;
    }
    Console::printf("Progress code %d\r\n", code);

    // digitalWrite(LED_BUILTIN, HIGH);
    if (code & 0x1)
        digitalWrite(LED_RED, LOW);
    else
        digitalWrite(LED_RED, HIGH);
    if (code & 0x2)
        digitalWrite(LED_GREEN, LOW);
    else
        digitalWrite(LED_GREEN, HIGH);
    if (code & 0x4)
        digitalWrite(LED_BLUE, LOW);
    else
        digitalWrite(LED_BLUE, HIGH);
}

[[noreturn]] void Debug::errorExit(const int code) {
    while (true) {
        Console::printf("Error code %d\r\n", code);
        for (int i = 0; i < code; i++) {
            digitalWrite(LED_RED, LOW);
            delay(200);
            digitalWrite(LED_RED, HIGH);
            delay(200);
        }
        delay(2000);
    }
}

void Debug::testOnboardLeds() {
    Console::println("testOnboardLeds: red");
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    delay(200);
    Console::println("testOnboardLeds: green");
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
    delay(200);
    Console::println("testOnboardLeds: blue");
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
    delay(200);
    // Console::println("testOnboardLeds: white");
    // digitalWrite(LED_RED, LOW);
    // digitalWrite(LED_GREEN, LOW);
    // digitalWrite(LED_BLUE, LOW);
    // delay(200);
}
