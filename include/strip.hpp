//
// Created by enrico on 21.06.22.
//

#ifndef _STRIP_HPP
#define _STRIP_HPP

#include "glob.hpp"
#include "micInput.hpp"

template <uint8_t ledPin, uint16_t ledCount>
class strip
{
private:
	CRGB leds[ledCount];

	bool reversed;

	// rainbow effects
	float colorOffset = 0;

	// pulse and stream colorPalette
	// CRGBPalette16 targetPalette = OceanColors_p;
	// CRGBPalette16 currentPalette = OceanColors_p;

	uint16_t peakDotFallSpeed;
	float colorChangeSpeed;

	// dynamic values
	// peak is used for peak dot, but also for Circle position
	float peak = 0;

	uint16_t calcHeight(uint16_t lvl, uint16_t minLvlAvg, uint16_t maxLvlAvg, uint16_t ledHeight = ledCount) const
	{
		uint16_t height = ledHeight * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

		if (height < 0)
			height = 0;
		else if (height > ledHeight)
			height = ledHeight;
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

	void fallPeakDot()
	{
		if (peak > 0)
			peak -= (float)ledCount / peakDotFallSpeed;
	}

	CHSV Wheel(uint16_t x, uint16_t _ledCount, float _offset = 240)
	{
		return {(map(x, 0, _ledCount - 1, 0, 255 * 2) - (int)_offset) % 255, 255, 255};
	}

public:
	explicit strip(bool reverse = false, uint8_t peakDotFallSpeed = 180, float colorChangeSpeed = 1)
	{
		this->reversed = reverse;
		this->peakDotFallSpeed = peakDotFallSpeed;
		this->colorChangeSpeed = colorChangeSpeed;
	}

	void init()
	{
		FastLED.addLeds<WS2812B, ledPin, GRB>(leds, ledCount);
		FastLED.setBrightness(255);
	}

	void Test()
	{
		leds[0] = CRGB::Aqua;
		leds[1] = CRGB::Red;
		leds[2] = CRGB::Aqua;
		leds[3] = CRGB::Red;
	}

	void Test2()
	{
		leds[0] = CRGB::AliceBlue;
		leds[1] = CRGB::Green;
		leds[2] = CRGB::Orange;
		leds[3] = CRGB::DarkRed;
	}

	void OffAnimiation()
	{
		for (uint16_t i = 0; i < ledCount; i++)
		{
			leds[i] = CRGB::Black;
		}

		leds[0] = CRGB::Red;
		leds[ledCount - 1] = CRGB::Red;
	}

	void Clear()
	{
		for (uint16_t i = 0; i < ledCount; i++)
		{
			leds[i] = CRGB::Black;
		}
	}

	void Normal(uint16_t lvl, uint16_t maxLvlAvg, bool rainbow = true, bool peakDot = true)
	{
		uint16_t height = calcHeight(lvl, 0, maxLvlAvg);

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

		if (peakDot && peak > 0)
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

		fallPeakDot();
	}

	void CentreOut(uint16_t lvl, uint16_t maxLvlAvg, bool rainbow = true, bool peakDot = true)
	{
		uint16_t height = calcHeight(lvl, 0, maxLvlAvg);

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

		// println(peak);

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
					// println(count - (uint16_t)peak);
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

		fallPeakDot();
	}

	void FallingStar(uint16_t lvl, uint16_t maxLvlAvg, bool rainbow = true, uint16_t width = 1)
	{
		uint16_t height = calcHeight(lvl, 0, maxLvlAvg, ledCount - width);
		if (rainbow)
			adjustColorOffsets();

		for (uint16_t i = 0; i < ledCount; i++)
		{
			leds[i] = CRGB::Black;
		}

		if (reversed)
		{
			for (uint16_t i = height; i < height + width; i++)
			{
				int j = (ledCount - 1) - i;
				if (rainbow)
				{
					leds[j] = CHSV(colorOffset, 255, 255);
				}
				else
				{
					leds[j] = CRGB::DarkCyan;
				}
			}
		}
		else
		{
			for (uint16_t i = height; i < height + width; i++)
			{

				if (rainbow)
				{
					leds[i] = CHSV(colorOffset, 255, 255);
				}
				else
				{
					leds[i] = CRGB::DarkCyan;
				}
			}
		}
	}

	void Circle(uint16_t lvl, uint16_t maxLvlAvg, uint8_t width = 1, float baseMove = 0.05, uint8_t bars = 1, float moveSpeed = 1.8, bool rainbow = true)
	{
		uint16_t height = calcHeight(lvl, 0, maxLvlAvg);
		if (rainbow)
			adjustColorOffsets();

		for (uint16_t i = 0; i < ledCount; i++)
		{
			leds[i] = CRGB::Black;
		}

		uint16_t speed = ((uint32_t)height * height) / (ledCount * 0.8); // convert linear height to exponential function
		peak += ((speed * moveSpeed) / 30) + baseMove;

		if (peak > ledCount)
			peak -= ledCount; // reset

		uint16_t cPeak = (uint16_t)peak;
		float split = (float)ledCount / bars;

		if (reversed)
		{
			for (uint16_t i = cPeak; i < cPeak + width; i++)
			{
				int j = ledCount - (i % ledCount);
				// % ledCount start at beginning if at end
				if (rainbow)
				{
					for (uint8_t c = 0; c < bars; c++)
					{
						leds[(j + (uint16_t)(split * c)) % ledCount] = Wheel(i, ledCount, colorOffset); // CHSV(colorOffset, 255, 255);
					}
				}
				else
				{
					for (uint8_t c = 0; c < bars; c++)
					{
						leds[(j + (uint16_t)(split * c)) % ledCount] = CHSV(colorOffset, 255, 255);
					}
				}
			}
		}
		else
		{
			for (uint16_t i = cPeak; i < cPeak + width; i++)
			{
				// % ledCount start at beginning if at end
				if (rainbow)
				{
					for (uint8_t c = 0; c < bars; c++)
					{
						leds[(i + (uint16_t)(split * c)) % ledCount] = Wheel(i, ledCount, colorOffset); // CHSV(colorOffset, 255, 255);
					}
				}
				else
				{
					for (uint8_t c = 0; c < bars; c++)
					{
						leds[(i + (uint16_t)(split * c)) % ledCount] = CHSV(colorOffset, 255, 255);
					}
				}
			}
		}
	}

	void Pulse(uint16_t lvl, uint16_t maxLvlAvg, float intensity, bool rainbow = true)
	{
		uint16_t height = calcHeight(lvl, 0, maxLvlAvg);

		adjustColorOffsets();

		if (height >= ledCount * intensity)
		{
			for (uint16_t i = 0; i < ledCount; i++)
			{
				if (rainbow)
					leds[i] = Wheel(i, ledCount, colorOffset);
				else
					leds[i] = CHSV(colorOffset, 255, 255);
			}
		}
		else
		{
			for (uint16_t i = 0; i < ledCount; i++)
			{
				leds[i] = CRGB::Black;
			}
		}
	}
};

void show()
{
	// FastLED.setBrightness(255);
	FastLED.show();
};

#endif //_STRIP_HPP
