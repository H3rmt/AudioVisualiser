#include <Arduino.h>
#include <FastLED.h>
// #include <IRremote.hpp>

#include "strip.hpp"
#include "micInput.hpp"

#define MICPIN A0
#define AVGPIN A1

#define REMOTEPIN 4

#define SUBLEDPIN 2
#define SUBLEDCOUNT 59

#define MIDLEDPIN1 3
#define MIDLEDPIN2 6
#define MIDLEDCOUNT 27

#define debugg



// UpdatesPerSecond
#define UPS 80

#ifdef debug
// SecondsUntilOff
#define SUO 1
#else
#define SUO 5
#endif


// #define SIDELEDPIN1 5
// #define SIDELEDPIN2 6
// #define SIDELEDCOUNT 31


#define chechTime1 long f = millis();
#define chechTime2                \
	Serial.print("time millis:"); \
	Serial.println(millis() - f);

#ifdef debug
#define print(d) Serial.print(d);
#define println(d) Serial.println(d);
#else
#define print(d)
#define println(d)
#endif