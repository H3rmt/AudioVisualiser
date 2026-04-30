#include <Arduino.h>
#include "Core.hpp"

#ifdef CDEBUG
void inline Console::print(const String &s) {
    Serial.print(s);
}

void inline Console::print(const char str[]) {
    Serial.print(str);
}

void inline Console::print(const int value) {
    Serial.print(value);
}

void inline Console::print(const float value) {
    Serial.print(value);
}

void inline Console::println(const String &s) {
    Serial.println(s);
}

void inline Console::println(const char str[]) {
    Serial.println(str);
}

void inline Console::println(const int value) {
    Serial.println(value);
}

void inline Console::println(const float value) {
    Serial.println(value);
}

// joinked from Print.cpp
size_t Console::printf(const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    char temp[64];
    char *buffer = temp;
    size_t len = vsnprintf(temp, sizeof(temp), format, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1) {
        buffer = new char[len + 1];
        va_start(arg, format);
        vsnprintf(buffer, len + 1, format, arg);
        va_end(arg);
    }
    len = Serial.write(reinterpret_cast<const uint8_t *>(buffer), len);
    if (buffer != temp) {
        delete[] buffer;
    }
    return len;
}
#else
void inline Console::print(const String &s) {
}
void inline Console::print(const char str[]) {
}
void inline Console::print(int value) {
}
void inline Console::println(const String &s) {
}
void inline Console::println(const char str[]) {
}
void inline Console::println(int value) {
}
size_t inline Console::printf(const char *format, ...) {
}
#endif
