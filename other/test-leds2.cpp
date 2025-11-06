
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

auto ledCount = 5;
auto ledPin = D2;

Adafruit_NeoPixel pixels(ledCount, ledPin, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(115200);
    pixels.begin();
    pixels.setBrightness(50); // Set brightness to 50%
    pixels.clear(); // Clear the strip
    pixels.show(); // Update the strip
    pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Set first LED to red
    pixels.show(); // Update the strip to show the change
}

void loop() {

}