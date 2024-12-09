//
// Created by enrico on 21.06.22.
//

#ifndef _STRIP_HPP
#define _STRIP_HPP

#include "Defs.hpp"
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

// template <uint8_t ledPin, uint16_t ledCount>
class Strip
{
private:
	Adafruit_NeoPixel pixels;
	bool reversed;
	bool adaptiveBrightness;

	// rainbow effects
	uint32_t colorOffset = 0;

	uint8_t ledCount;

	// pulse and stream colorPalette
	// CRGBPalette16 targetPalette = OceanColors_p;
	// CRGBPalette16 currentPalette = OceanColors_p;

	// uint16_t peakDotFallSpeed;
	uint16_t colorChangeSpeed;

	// dynamic values
	// peak is used for peak dot, but also for Circle position
	float peak = 0;
	float circle_position = 0;
	uint16_t pulse_last = 10000000;

	uint16_t calcHeight(uint16_t lvl, uint16_t minLvlAvg, uint16_t maxLvlAvg) const
	{
		uint16_t height = ledCount * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

		if (height < 0)
			height = 0;
		else if (height > ledCount)
			height = ledCount;
		return height;
	}

	void adjustColorOffsets()
	{
		colorOffset += colorChangeSpeed;
		if (colorOffset >= 65535)
		{
			colorOffset = 0;
		}
	}

	void setBrightness(uint16_t lvl, uint16_t maxLvlAvg)
	{
		if (!adaptiveBrightness)
			pixels.setBrightness(100);
		else
		{
			uint32_t brightness = map(lvl, 0, maxLvlAvg, 20, 400);
			uint32_t brightness_c = max(25, ((uint32_t)(brightness / 5)) * 5);
			if (brightness_c > 255)
				brightness_c = 255;
			pixels.setBrightness(brightness_c);
		}
	}

	// void fallPeakDot()
	// {
	// 	if (peak > 0)
	// 		peak -= (float)ledCount / peakDotFallSpeed;
	// }

	uint32_t Wheel(uint16_t x, uint8_t _ledCount)
	{
		return Adafruit_NeoPixel::ColorHSV((map(x, 0, _ledCount, 0, 65535 * 2) + colorOffset) % 65535, 255, 255);
	}

public:
	explicit Strip(uint8_t ledPin, uint16_t ledCount, bool reverse = false, uint8_t peakDotFallSpeed = 180, uint16_t colorChangeSpeed = 500, bool adaptiveBrightness = false)
		: pixels(ledCount, ledPin, NEO_GRB + NEO_KHZ800), ledCount(ledCount)
	{
		this->reversed = reverse;
		// this->peakDotFallSpeed = peakDotFallSpeed;
		this->colorChangeSpeed = colorChangeSpeed;
		this->adaptiveBrightness = adaptiveBrightness;
	}

	void init()
	{
		pixels.begin();
		pixels.setBrightness(70);
	}

	void Test()
	{
		for (uint16_t i = 0; i < ledCount; i++)
		{
			pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
		}

		pixels.setPixelColor(0, Adafruit_NeoPixel::Color(100, 0, 200));
		pixels.setPixelColor(1, Adafruit_NeoPixel::Color(200, 40, 0));
		pixels.setPixelColor(2, Adafruit_NeoPixel::Color(100, 0, 200));
		pixels.setPixelColor(3, Adafruit_NeoPixel::Color(200, 40, 0));

		pixels.setPixelColor(ledCount - 1, Adafruit_NeoPixel::Color(80, 40, 200));
		pixels.setPixelColor(ledCount - 2, Adafruit_NeoPixel::Color(180, 40, 100));
		pixels.setPixelColor(ledCount - 3, Adafruit_NeoPixel::Color(240, 0, 0));
		pixels.show();
	}

	void OffAnimiation()
	{
		for (uint16_t i = 0; i < ledCount; i++)
		{
			pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
		}

		pixels.setPixelColor(0, Adafruit_NeoPixel::Color(200, 0, 0));
		pixels.setPixelColor(ledCount - 1, Adafruit_NeoPixel::Color(200, 0, 0));
		pixels.show();
	}

	void Clear()
	{
		for (uint16_t i = 0; i < ledCount; i++)
		{
			pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
		}
	}

	void Normal(uint32_t lvl, uint32_t maxLvlAvg, bool rainbow = false, bool peakDot = false)
	{
		uint8_t height = map(lvl, 0, maxLvlAvg, 0, ledCount);
		adjustColorOffsets();

		if (reversed)
		{
			for (uint16_t i = 0; i < ledCount; i++)
			{
				int j = (ledCount - 1) - i;
				if (i >= height)
					pixels.setPixelColor(j, Adafruit_NeoPixel::Color(0, 0, 0));
				else
				{
					if (rainbow)
					{
						pixels.setPixelColor(j, Wheel(i, ledCount));
					}
					else
					{
						pixels.setPixelColor(j, Adafruit_NeoPixel::Color(0, 0, 255));
					}
				}
			}
		}
		else
		{
			for (uint16_t i = 0; i < ledCount; i++)
			{
				if (i >= height)
					pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
				else
				{
					if (rainbow)
					{
						pixels.setPixelColor(i, Wheel(i, ledCount));
					}
					else
					{
						pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 255));
					}
				}
			}
		}

		setBrightness(lvl, maxLvlAvg);
		pixels.show();
	}

	void CentreOut(uint16_t lvl, uint16_t maxLvlAvg, bool rainbow = false, bool peakDot = false)
	{
		uint8_t height = map(lvl, 0, maxLvlAvg, 0, ledCount);
		adjustColorOffsets();

		// used instead of ledCount, but can vary depending on %2 on ledCount
		uint16_t count = 0;

		if (ledCount % 2 == 0)
		{
			height /= 2;
			count = ledCount / 2;
			for (uint16_t i = 0; i < count; i++)
			{
				if (i >= height)
				{
					pixels.setPixelColor(count - i - 1, Adafruit_NeoPixel::Color(0, 0, 0));
					pixels.setPixelColor(count + i, Adafruit_NeoPixel::Color(0, 0, 0));
				}
				else
				{
					if (rainbow)
					{
						uint32_t color = Wheel(i, ledCount);
						pixels.setPixelColor(count - i - 1, color);
						pixels.setPixelColor(count + i, color);
					}
					else
					{
						uint32_t color = Adafruit_NeoPixel::Color(0, 0, 255);
						pixels.setPixelColor(count - i - 1, color);
						pixels.setPixelColor(count + i, color);
					}
				}
			}
		}
		else
		{
			height = (height + 1) / 2;
			count = (ledCount + 1) / 2;

			for (uint16_t i = 0; i < count; i++)
			{
				if (i >= height)
				{
					pixels.setPixelColor(count - i - 1, Adafruit_NeoPixel::Color(0, 0, 0));
					pixels.setPixelColor(count + i - 1, Adafruit_NeoPixel::Color(0, 0, 0));
				}
				else
				{
					if (rainbow)
					{
						uint32_t color = Wheel(i, ledCount);
						pixels.setPixelColor(count - i - 1, color);
						pixels.setPixelColor(count + i - 1, color);
					}
					else
					{
						uint32_t color = Adafruit_NeoPixel::Color(0, 0, 255);
						pixels.setPixelColor(count - i - 1, color);
						pixels.setPixelColor(count + i - 1, color);
					}
				}
			}
		}

		setBrightness(lvl, maxLvlAvg);
		pixels.show();
	}

	void CentreIn(uint16_t lvl, uint16_t maxLvlAvg, bool rainbow = false, bool peakDot = false)
	{
		uint8_t height = map(lvl, 0, maxLvlAvg, 0, ledCount);
		adjustColorOffsets();

		// used instead of ledCount, but can vary depending on %2 on ledCount
		uint16_t count = 0;

		if (ledCount % 2 == 0)
		{
			height /= 2;
			count = ledCount / 2;
			for (uint16_t i = 0; i < count; i++)
			{
				if (i >= height)
				{
					pixels.setPixelColor(ledCount - i - 1, Adafruit_NeoPixel::Color(0, 0, 0));
					pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
				}
				else
				{
					if (rainbow)
					{
						uint32_t color = Wheel(i, ledCount);
						pixels.setPixelColor(ledCount - i - 1, color);
						pixels.setPixelColor(i, color);
					}
					else
					{
						uint32_t color = Adafruit_NeoPixel::Color(0, 0, 255);
						pixels.setPixelColor(ledCount - i - 1, color);
						pixels.setPixelColor(i, color);
					}
				}
			}
		}
		else
		{
			height = (height + 1) / 2;
			count = (ledCount + 1) / 2;

			for (uint16_t i = 0; i < count; i++)
			{
				if (i >= height)
				{
					pixels.setPixelColor(ledCount - 1 - i, Adafruit_NeoPixel::Color(0, 0, 0));
					pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
				}
				else
				{
					if (rainbow)
					{
						uint32_t color = Wheel(i, ledCount);
						pixels.setPixelColor(ledCount - 1 - i, color);
						pixels.setPixelColor(i, color);
					}
					else
					{
						uint32_t color = Adafruit_NeoPixel::Color(0, 0, 255);
						pixels.setPixelColor(ledCount - 1 - i, color);
						pixels.setPixelColor(i, color);
					}
				}
			}
		}

		setBrightness(lvl, maxLvlAvg);
		pixels.show();
	}

	// void FallingStar(uint16_t lvl, uint16_t maxLvlAvg, bool rainbow = true, uint16_t width = 1)
	// {
	// 	uint16_t height = calcHeight(lvl, 0, maxLvlAvg, ledCount - width);
	// 	if (rainbow)
	// 		adjustColorOffsets();

	// 	for (uint16_t i = 0; i < ledCount; i++)
	// 	{
	// 		leds[i] = CRGB::Black;
	// 	}

	// 	if (reversed)
	// 	{
	// 		for (uint16_t i = height; i < height + width; i++)
	// 		{
	// 			int j = (ledCount - 1) - i;
	// 			if (rainbow)
	// 			{
	// 				leds[j] = CHSV(colorOffset, 255, 255);
	// 			}
	// 			else
	// 			{
	// 				leds[j] = CRGB::DarkCyan;
	// 			}
	// 		}
	// 	}
	// 	else
	// 	{
	// 		for (uint16_t i = height; i < height + width; i++)
	// 		{

	// 			if (rainbow)
	// 			{
	// 				leds[i] = CHSV(colorOffset, 255, 255);
	// 			}
	// 			else
	// 			{
	// 				leds[i] = CRGB::DarkCyan;
	// 			}
	// 		}
	// 	}
	// }

	void Pulse(int16_t lvl, uint16_t maxLvlAvg, bool onlyPeak = false)
	{
			pulse_last = lvl;
		if (onlyPeak && (lvl < maxLvlAvg * 0.80 && lvl < pulse_last * 1.3))
		{
			if (peak >= 5)
			{
				for (uint16_t i = 0; i < ledCount; i++)
				{
					pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
				}
				setBrightness(0, 1000);
				pixels.show();
				return;
			}
			peak++;
		}
		else
		{
			peak = 0;
		}

		uint32_t speed = map(lvl, 0, maxLvlAvg, 0, colorChangeSpeed * 9);
		float speed_c = (speed * speed) / (colorChangeSpeed * 7);
		colorOffset += speed_c;
		if (colorOffset >= 65535)
		{
			colorOffset = 0;
		}

		uint32_t color = Adafruit_NeoPixel::ColorHSV(colorOffset, 255, 255);
		for (uint16_t i = 0; i < ledCount; i++)
		{
			pixels.setPixelColor(i, color);
		}

		setBrightness(lvl, maxLvlAvg);
		pixels.show();
	}

	void Circle(uint16_t lvl, uint16_t maxLvlAvg, uint8_t width = 5, uint8_t bars = 2, float moveSpeed = 0.1, bool rainbow = true)
	{
		uint32_t height = map(lvl, 0, maxLvlAvg, 0, ledCount);
		// Serial.println(height);
		adjustColorOffsets();

		for (uint16_t i = 0; i < ledCount; i++)
		{
			pixels.setPixelColor(i, Adafruit_NeoPixel::Color(0, 0, 0));
		}
		peak *= 0.90;

		float speed_1 = (height * height) / ((float)ledCount * 1.1);
		float speed_2 = (float)height * .8;
		float speed = max(speed_1, speed_2);
		float add = (speed * moveSpeed) + (moveSpeed / 2);
		if (add > peak)
			peak = add;

		circle_position += peak;

		if (circle_position > ledCount)
			circle_position -= ledCount; // reset

		float split = (float)ledCount / bars;

		uint32_t static_color = speed_1 > speed_2 ? Adafruit_NeoPixel::Color(0, 0, 255) : Adafruit_NeoPixel::Color(0, 255, 0);

		if (reversed)
		{
			for (uint16_t i = circle_position; i < circle_position + width; i++)
			{
				// % ledCount start at beginning if at end
				if (rainbow)
				{
					for (uint8_t c = 0; c < bars; c++)
					{
						pixels.setPixelColor((ledCount - 1 - (i + (uint16_t)(split * c))) % ledCount, Wheel(i, ledCount));
					}
				}
				else
				{
					for (uint8_t c = 0; c < bars; c++)
					{
						pixels.setPixelColor((ledCount - 1 - (i + (uint16_t)(split * c))) % ledCount, static_color);
					}
				}
			}
		}
		else
		{
			for (uint16_t i = circle_position; i < circle_position + width; i++)
			{
				// % ledCount start at beginning if at end
				if (rainbow)
				{
					for (uint8_t c = 0; c < bars; c++)
					{
						pixels.setPixelColor((i + (uint16_t)(split * c)) % ledCount, Wheel(i, ledCount));
					}
				}
				else
				{
					for (uint8_t c = 0; c < bars; c++)
					{
						pixels.setPixelColor((i + (uint16_t)(split * c)) % ledCount, static_color);
					}
				}
			}
		}

		setBrightness(lvl, maxLvlAvg);
		pixels.show();
	}

	// void Pulse(uint16_t lvl, uint16_t maxLvlAvg, float intensity, bool rainbow = true)
	// {
	// 	uint16_t height = calcHeight(lvl, 0, maxLvlAvg);

	// 	adjustColorOffsets();

	// 	if (height >= ledCount * intensity)
	// 	{
	// 		for (uint16_t i = 0; i < ledCount; i++)
	// 		{
	// 			if (rainbow)
	// 				leds[i] = Wheel(i, ledCount, colorOffset);
	// 			else
	// 				leds[i] = CHSV(colorOffset, 255, 255);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		for (uint16_t i = 0; i < ledCount; i++)
	// 		{
	// 			leds[i] = CRGB::Black;
	// 		}
	// 	}
	// }
};

// void show()
// {
// 	// int avg = analogRead(AVGPIN);
// 	int avg = 600;

// 	FastLED.setBrightness(map(avg, 0, 730, 0, 250));
// 	FastLED.show();
// };

#endif //_STRIP_HPP