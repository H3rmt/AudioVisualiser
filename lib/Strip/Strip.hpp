#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class Strip
{
public:
	Strip(uint16_t ledCount, int16_t ledPin);
	void begin();

	void setReversed(bool reverse);
	void setRainbow(bool rainbow);
	void setAdaptiveBrightness(bool adaptive);
	void setColorChangeSpeed(uint16_t speed);
	void setMaxBrightness(uint8_t maxBrightness);

	void test();
	void test2();
	void test3();
	void offAnimiation();

	void normal(uint32_t lvl, uint32_t maxLvlAvg);
	void centre(uint32_t lvl, uint32_t maxLvlAvg);
	void pulse(uint32_t lvl, uint32_t maxLvlAvg, bool onlyPeak);
	void circle(uint32_t lvl, uint32_t maxLvlAvg, uint16_t width, uint16_t bars, float moveSpeed, bool reverseOnPeak);

private:
	Adafruit_NeoPixel pixels;
	uint16_t ledCount;

	bool reversed;
	bool rainbow;
	bool adaptiveBrightness;
	uint16_t colorChangeSpeed;
	uint8_t maxBrightness;

	struct OffAnimState
	{
		float circle_position = 0;
	};
	OffAnimState offAnimState;

	struct PulseAnimState
	{
		uint32_t pulse_last = 10000; // last pulse level
		uint8_t isNoPeak = 0;		 // peak counter
	};
	PulseAnimState pulseAnimState;

	struct CircleAnimState
	{
		float movement = 0.0;		  // movement speed
		uint32_t last_dir_change = 0; // last direction change
		uint32_t circlePosition = 0; // current position in the circle
	};
	CircleAnimState circleAnimState;

	struct GlobalState
	{
		uint32_t colorOffset = 0;
	};
	GlobalState globalState;

	void clear();
	void updateColorOffset();
	void setBrightness(uint16_t lvl, uint16_t maxLvlAvg);
};