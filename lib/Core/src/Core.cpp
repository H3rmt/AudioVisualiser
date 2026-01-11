#include <Arduino.h>
#include "Core.hpp"

#ifdef CDEBUG
void Console::print(const String &s)
{
    Serial.print(s);
}
void Console::print(const char str[])
{
    Serial.print(str);
}
void Console::print(const int value)
{
    Serial.print(value);
}
void Console::println(const String &s)
{
    Serial.println(s);
}
void Console::println(const char str[])
{
    Serial.println(str);
}
void Console::println(const int value)
{
    Serial.println(value);
}
void Console::printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Serial.vprintf(format, args);
    va_end(args);
}
#else
void Console::print(const String &s) {}
void Console::print(const char str[]) {}
void Console::print(int value) {}
void Console::println(const String &s) {}
void Console::println(const char str[]) {}
void Console::println(int value) {}
void Console::printf(const char *format, ...) {}
#endif