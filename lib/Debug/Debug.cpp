#include <Arduino.h>

void setupOnboardLeds()
{
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}

/// @brief color led to show progress in code
/// 1 = red
/// 2 = green
/// 3 = yellow
/// 4 = blue
/// 5 = purple
/// 6 = cyan
/// 7 = white
/// @param code
void progress(int code)
{
    if (code == 0)
    {
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_BLUE, HIGH);
        return;
    }

    digitalWrite(LED_BUILTIN, HIGH);
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
    delay(200);
}

void errorExit(int code)
{
    while (1)
    {
        for (int i = 0; i < code; i++)
        {
            digitalWrite(LED_RED, LOW);
            delay(200);
            digitalWrite(LED_RED, HIGH);
            delay(200);
        }
        delay(2000);
    }
}

void onboardLeds()
{
    Serial.println("LOOP 0");
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    delay(500);
    Serial.println("LOOP 1");
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
    delay(500);
    Serial.println("LOOP 2");
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
    delay(500);
}