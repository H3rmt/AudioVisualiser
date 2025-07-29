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

	uint16_t colorChangeSpeed;

	// dynamic values
	// peak is used for peak dot, but also for Circle position
	float peak = 0;
	uint16_t last_dir_change = 0;
	float circle_position = 0;
	uint16_t pulse_last = 10000;

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
			pixels.setBrightness(70);
		else
		{
			uint32_t brightness = map(lvl, 0, maxLvlAvg, 20, 300);
			uint32_t brightness_c = min(255, max(25, ((uint32_t)(brightness / 5)) * 5));
			pixels.setBrightness(brightness_c);
		}
	}

	uint32_t Wheel(uint16_t x, uint8_t _ledCount)
	{
		return Adafruit_NeoPixel::ColorHSV((map(x, 0, _ledCount, 0, 65535 * 2) + colorOffset) % 65535, 255, 255);
	}

public:
	explicit Strip(uint8_t ledPin, uint16_t ledCount, bool reverse = false, uint16_t colorChangeSpeed = 500, bool adaptiveBrightness = false)
		: pixels(ledCount, ledPin, NEO_RGB + NEO_KHZ800), ledCount(ledCount)
	{
		this->reversed = reverse;
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

		uint16_t start = ((uint16_t)(circle_position)) % ledCount;
		pixels.setPixelColor(start, Adafruit_NeoPixel::Color(200, 0, 0));
		pixels.setPixelColor((start + ledCount - 1) % ledCount, Adafruit_NeoPixel::Color(200, 0, 0));
		pixels.show();
		circle_position += 0.05;
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

	void Pulse(uint16_t lvl, uint16_t maxLvlAvg, bool onlyPeak = false)
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
		last_dir_change++;

		// TF is this calculation
		float speed_1 = (height * height) / ((float)ledCount); // blue
		float speed_2 = (float)height * .8; // green 
		float speed = max(speed_1, speed_2);
		float add = (speed * moveSpeed) + (moveSpeed / 2);
		uint32_t static_color = speed_1 > speed_2 ? Adafruit_NeoPixel::Color(0, 0, 255) : Adafruit_NeoPixel::Color(0, 255, 0);

		if (add > peak)
			peak = add;

		if (lvl > maxLvlAvg && height >= ledCount - 2 && last_dir_change > 10)
		{
			last_dir_change = 0;
			static_color = Adafruit_NeoPixel::Color(255, 0, 0); // red
			reversed = !reversed;
		}
		circle_position += peak;

		if (circle_position > ledCount)
			circle_position -= ledCount; // reset

		uint8_t bar_offset = (float)ledCount / bars;


		Serial.println();
		for (uint16_t i = 0; i < width; i++)
		{
			for (uint8_t c = 0; c < bars; c++)
			{
				uint16_t pos = (i + (uint16_t)circle_position + (bar_offset * c)) % ledCount;
				if (reversed)
				{
					pos = (ledCount - 1 - pos);
				}
				if (rainbow)
				{
					pixels.setPixelColor(pos, Wheel(i, ledCount));
				}
				else
				{
					pixels.setPixelColor(pos, static_color);
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