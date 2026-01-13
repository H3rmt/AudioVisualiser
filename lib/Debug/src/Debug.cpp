#include <Arduino.h>
#include <Core.hpp>

#include "Debug.hpp"

#define LED_RED D0
#define LED_WHITE D1
#define LED_BLUE D2
#define LED_GREEN D3


void Debug::setupOnboardLeds() {
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_WHITE, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);

    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_WHITE, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
}

void Debug::progress(const int code) {
    Console::printf("Progress code %d\r\n", code);
    digitalWrite(LED_WHITE, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);

    if (code == 0) {
        return;
    }

    // digitalWrite(LED_BUILTIN, HIGH);
    if (code & 0x1)
        digitalWrite(LED_WHITE, HIGH);
    else
        digitalWrite(LED_WHITE, LOW);
    if (code & 0x2)
        digitalWrite(LED_BLUE, HIGH);
    else
        digitalWrite(LED_BLUE, LOW);
    if (code & 0x4)
        digitalWrite(LED_GREEN, HIGH);
    else
        digitalWrite(LED_GREEN, LOW);
}

[[noreturn]] void Debug::errorExit(const int code) {
    while (true) {
        Console::printf("Error code %d\r\n", code);
        for (int i = 0; i < code; i++) {
            digitalWrite(LED_RED, HIGH);
            delay(250);
            digitalWrite(LED_RED, LOW);
            delay(250);
        }
        delay(3000);
    }
}

void Debug::testOnboardLeds() {
    Console::println("testOnboardLeds: red");
    digitalWrite(LED_WHITE, HIGH);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
    delay(200);
    Console::println("testOnboardLeds: green");
    digitalWrite(LED_WHITE, LOW);
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, LOW);
    delay(200);
    Console::println("testOnboardLeds: blue");
    digitalWrite(LED_WHITE, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, HIGH);
    delay(200);
}

void Debug::loop() {
    while (true) ;
}