//
// Created by enrico on 21.06.22.
//

#ifndef _STRIP_HPP
#define _STRIP_HPP

#include <Arduino.h>
#include "FastLED.h"
#include "micInput.hpp"

template <uint8_t ledPin, uint16_t ledCount>
class strip
{
private:
	CRGB leds[ledCount]{};

	bool reversed;

	// rainbow effects
	uint16_t colorOffset = 0;

	// pulse and stream colorPalette
	CRGBPalette16 targetPalette = OceanColors_p;
	CRGBPalette16 currentPalette = OceanColors_p;

	uint16_t peakDotFallSpeed;
	uint8_t colorChangeSpeed;

	// dynamic values
	float peak = 0;
	uint8_t peakDotCount = 0;

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
		if (colorOffset >= 255)
		{
			colorOffset = 0;
		}
	}

	void fallPeakDot(uint16_t fallSpeed)
	{
		if (peakDotCount > 4)
		{
			if (peak > 0)
				peak -= (float)ledCount / fallSpeed;
			peakDotCount = 0;
		}
		else
			peakDotCount++;
	}

	CHSV Wheel(uint16_t x, uint16_t _ledCount, uint8_t _offset = 240)
	{
		return {(map(x, 0, _ledCount - 1, 0, 255 * 2) - _offset) % 255, 255, 255};
	}

public:
	explicit strip(bool reverse = false, uint16_t peakDotFallDivide = 50, uint8_t colorChangeSpeed = 1)
	{
		this->reversed = reverse;
		this->peakDotFallSpeed = peakDotFallDivide;
		this->colorChangeSpeed = colorChangeSpeed;
	}

	void init()
	{
		FastLED.addLeds<WS2812B, ledPin, GRB>(leds, ledCount);
		FastLED.setBrightness(255);
	}

	void show()
	{
		FastLED.show();
	}

	void Test()
	{
		leds[0] = CRGB::Aqua;
		leds[1] = CRGB::Red;
		leds[2] = CRGB::Aqua;
		leds[3] = CRGB::Red;
		show();
	}

	void Test2()
	{
		leds[0] = CRGB::AliceBlue;
		leds[1] = CRGB::Green;
		leds[2] = CRGB::Orange;
		leds[3] = CRGB::DarkRed;
		show();
	}
	


	void Clear()
	{
		for (uint16_t i = 0; i < ledCount; i++)
		{
			leds[i] = CRGB::Black;
		}
		show();
	}

	void Normal(bool rainbow, uint16_t lvl, uint16_t minLvlAvg, uint16_t maxLvlAvg)
	{

		uint16_t height = calcHeight(lvl, minLvlAvg, maxLvlAvg);

		if (rainbow)
			adjustColorOffsets();

		if (reversed)
		{
			for (uint16_t i = 0; i < ledCount; i++)
			{
				int j = (ledCount - 1) - i;
				if (i >= height)
					leds[j] = CRGB::Black;
				else
				{
					if (rainbow)
					{
						leds[j] = Wheel(i, ledCount, colorOffset);
					}
					else
					{
						leds[j] = Wheel(i, ledCount);
					}
				}
			}
		}
		else
		{
			for (uint16_t i = 0; i < ledCount; i++)
			{
				if (i >= height)
					leds[i] = CRGB::Black;
				else
				{
					if (rainbow)
					{
						leds[i] = Wheel(i, ledCount, colorOffset);
					}
					else
					{
						leds[i] = Wheel(i, ledCount);
					}
				}
			}
		}
		if (height > peak)
			peak = height;
		if (peak >= ledCount)
			peak = ledCount - 1;
		if (peak > 0)
		{
			if (rainbow)
			{
				if (reversed)
				{
					leds[ledCount - (uint16_t)peak] = Wheel((uint16_t)peak, ledCount, colorOffset);
				}
				else
				{
					leds[(uint16_t)peak] = Wheel((uint16_t)peak, ledCount, colorOffset);
				}
			}
			else
			{
				if (reversed)
				{
					leds[ledCount - (uint16_t)peak] = Wheel((uint16_t)peak, ledCount);
				}
				else
				{
					leds[(uint16_t)peak] = Wheel((uint16_t)peak, ledCount);
				}
			}
		}

		show();

		fallPeakDot(peakDotFallSpeed);
	}

	void CentreOut(bool rainbow, uint16_t lvl, uint16_t minLvlAvg, uint16_t maxLvlAvg)
	{
		uint16_t height = calcHeight(lvl, minLvlAvg, maxLvlAvg);

		if (rainbow)
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
					leds[count - i - 1] = CRGB::Black;
					leds[count + i] = CRGB::Black;
				}
				else
				{
					if (rainbow)
					{
						CRGB color = Wheel(i, ledCount, colorOffset);
						leds[count - i - 1] = color;
						leds[count + i] = color;
					}
					else
					{
						CRGB color = Wheel(i, ledCount);
						leds[count - i - 1] = color;
						leds[count + i] = color;
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
					leds[count - i - 1] = CRGB::Black;
					leds[count + i - 1] = CRGB::Black;
				}
				else
				{
					if (rainbow)
					{
						CRGB color = Wheel(i, ledCount, colorOffset);
						leds[count - i - 1] = color;
						leds[count + i - 1] = color;
					}
					else
					{
						CRGB color = Wheel(i, ledCount);
						leds[count - i - 1] = color;
						leds[count + i - 1] = color;
					}
				}
			}
		}
		if (height > peak)
			peak = height;
		if (peak >= ledCount)
			peak = ledCount - 1;

		if (peak > 0)
		{
			if (rainbow)
			{
				if (ledCount % 2 == 0)
				{
					CRGB color = Wheel((uint16_t)peak, ledCount, colorOffset);
					leds[count - (uint16_t)peak] = color;
					leds[count + (uint16_t)peak - 1] = color;
				}
				else
				{
					CRGB color = Wheel((uint16_t)peak, ledCount, colorOffset);
					leds[count - (uint16_t)peak] = color;
					leds[count + (uint16_t)peak - 2] = color;
				}
			}
			else
			{
				if (ledCount % 2 == 0)
				{
					CRGB color = Wheel((uint16_t)peak, ledCount);
					leds[count - (uint16_t)peak] = color;
					leds[count + (uint16_t)peak - 1] = color;
				}
				else
				{
					CRGB color = Wheel((uint16_t)peak, ledCount);
					leds[count - (uint16_t)peak] = color;
					leds[count + (uint16_t)peak - 2] = color;
				}
			}
		}

		show();

		fallPeakDot(peakDotFallSpeed);
	}

	void FallingStar(uint16_t lvl, uint16_t minLvlAvg, uint16_t maxLvlAvg)
	{
		uint16_t height = calcHeight(lvl, minLvlAvg, maxLvlAvg);

		for (uint16_t i = 0; i < ledCount; i++)
		{
			leds[i] = CRGB::Black;
		}

		if (height > peak)
			peak = height;
		if (peak >= ledCount)
			peak = ledCount - 1;

		if (peak > 0)
		{
			if (reversed)
				leds[ledCount - (uint16_t)peak] = CRGB::Blue;
			else
				leds[(uint16_t)peak] = CRGB::Blue;
		}

		show();

		fallPeakDot(peakDotFallSpeed / 3);
	}

	void Pulse(uint16_t lvl, uint16_t minLvlAvg, uint16_t maxLvlAvg)
	{
		EVERY_N_SECONDS(5)
		{
			for (int i = 0; i < 16; i++)
			{
				targetPalette[i] = CHSV(colorOffset, 255, 255);
			}
		}

		EVERY_N_MILLISECONDS(100)
		{
			nblendPaletteTowardPalette(currentPalette, targetPalette, 20);
		}

		EVERY_N_MILLISECONDS(20)
		{
			// fadeToBlackBy(leds, ledCount, 4);

			uint16_t height = 250 * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

			auto newcolour = ColorFromPalette(currentPalette, constrain(height, 0, 255), constrain(height + 5 + random8(40), 0, 255), LINEARBLEND);

			if (ledCount % 2 == 0)
			{
				nblend(leds[ledCount / 2], newcolour, 128);
				nblend(leds[ledCount / 2 - 1], newcolour, 128);
			}
			else
			{
				nblend(leds[ledCount / 2], newcolour, 128);
			}

			for (uint16_t i = ledCount - 1; i > ledCount / 2; i--)
			{
				leds[i] = leds[i - 1];
			}
			for (uint16_t i = 0; i < ledCount / 2; i++)
			{
				leds[i] = leds[i + 1];
			}

			if (lvl > (minLvlAvg + ((maxLvlAvg - minLvlAvg) * 0.5)) && random8(10) > 5)
			{
				leds[random16(ledCount)] += CRGB::White;
			}

			show();

			adjustColorOffsets();
		}
	}

	void Stream(uint16_t lvl, uint16_t minLvlAvg, uint16_t maxLvlAvg)
	{
		EVERY_N_SECONDS(5)
		{
			for (int i = 0; i < 16; i++)
			{
				targetPalette[i] = CHSV(random8(), 255, 255);
			}
		}

		EVERY_N_MILLISECONDS(100)
		{
			nblendPaletteTowardPalette(currentPalette, targetPalette, 20);
		}

		EVERY_N_MILLISECONDS(20)
		{
			fadeToBlackBy(leds, ledCount, ledCount / 5);

			auto newColor = ColorFromPalette(currentPalette, constrain(lvl, 0, 255), constrain(lvl * 2, 0, 255),
											 LINEARBLEND);
			if (reversed)
				nblend(leds[ledCount - 1], newColor, 128);
			else
				nblend(leds[0], newColor, 128);

			if (reversed)
				for (uint16_t i = ledCount - 1; i > 0; i--)
				{
					leds[i] = leds[i - 1];
				}
			else
				for (uint16_t i = 0; i < ledCount; i++)
				{
					leds[i] = leds[i + 1];
				}
			if (lvl > (minLvlAvg + ((maxLvlAvg - minLvlAvg) * 0.5)) && random8(10) > 5)
			{
				leds[random16(ledCount)] += CRGB::White;
			}

			show();
		}
	}

	void Maximum(bool rainbow, uint16_t lvl, uint16_t minLvlAvg, uint16_t maxLvlAvg)
	{
		uint16_t height = calcHeight(maxLvlAvg, 0, 1023);
		if (rainbow)
			adjustColorOffsets();

		if (reversed)
		{
			for (uint16_t i = 0; i < ledCount; i++)
			{
				int j = (ledCount - 1) - i;
				if (i >= height)
					leds[j] = CRGB::Black;
				else
				{
					if (rainbow)
					{
						leds[j] = Wheel(i, ledCount, colorOffset);
					}
					else
					{
						leds[j] = Wheel(i, ledCount);
					}
				}
			}
		}
		else
		{
			for (uint16_t i = 0; i < ledCount; i++)
			{
				if (i >= height)
					leds[i] = CRGB::Black;
				else
				{
					if (rainbow)
					{
						leds[i] = Wheel(i, ledCount, colorOffset);
					}
					else
					{
						leds[i] = Wheel(i, ledCount);
					}
				}
			}
		}

		show();
	}
};

#endif //_STRIP_HPP
