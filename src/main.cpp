//
// Created by enrico on 22.06.22.
//

#include "strip.hpp"
#include "micInput.hpp"

#define SAMPLES 25

#define LEDPIN 4
#define LEDCOUNT 34
#define BUTTONPIN 3

#define SUBLEDPIN 3
#define SUBLEDCOUNT 59
#define SUBBUTTONPIN 9

#define MIDLEDPIN 2
#define MIDLEDCOUNT 27
#define MIDBUTTONPIN 10

micinput<SAMPLES> input = micinput<SAMPLES>(A0, -100, 45, 30, 100);
strip<SUBLEDPIN, SUBLEDCOUNT> sub = strip<SUBLEDPIN, SUBLEDCOUNT>(false, 50, 1.5, 1.5);
strip<MIDLEDPIN, MIDLEDCOUNT> mid = strip<MIDLEDPIN, MIDLEDCOUNT>(false, 50, 1.5, 1.5);

int subState = 6;
int midState = 3;

void setup()
{
	Serial.begin(9600);
	input.init();

	sub.init();
	mid.init();

	// sub.Test2();
	// delay(400);

	// mid.Test2();
	// delay(400);

	// mid.Test();
	// delay(400);

	// sub.Test();
	// delay(400);

	pinMode(SUBBUTTONPIN, INPUT);
	pinMode(MIDBUTTONPIN, INPUT);
}

void loop()
{
	input.read();

	if (digitalRead(SUBBUTTONPIN))
	{
		subState++;
		// Serial.println(subState);
		while (digitalRead(SUBBUTTONPIN))
		{
		}
	}

	if (digitalRead(MIDBUTTONPIN))
	{
		midState++;
		// Serial.println(midState);
		while (digitalRead(MIDBUTTONPIN))
		{
		}
	}

	if (subState >= 7)
		subState = 0;

	if (midState >= 7)
		midState = 0;

	// sub.Test();

	// sub.Normal(input.getLvl(), input.getAvg(), true, true);
	// mid.Normal(input.getLvl(), input.getAvg(), true, true);
	// sub.Normal(input.getMax(), 611, false, false);
	// mid.Normal(input.getAvg(), 611, false, false);
	// mid.Normal(input.getRaw(), input.getAvg(), true, true);
	// mid.Normal(input.getLvl(), 611, false, false);

	// return;

	switch (subState)
	{
	case 0:
		sub.Clear();
		break;
	case 1:
		sub.Normal(input.getLvl(), input.getAvg(), false);
		break;
	case 2:
		sub.CentreOut(input.getLvl(), input.getAvg(), false);
		break;
	case 3:
		sub.Normal(input.getLvl(), input.getAvg());
		break;
	case 4:
		sub.CentreOut(input.getLvl(), input.getAvg());
		break;
	case 5:
		sub.FallingStar(input.getLvl(), input.getAvg(), true, 3);
		break;
	case 6:
		sub.Circle(input.getLvl(), input.getAvg(), true, 8, 0.03);
		break;
	}

	// return;

	switch (midState)
	{
	case 0:
		mid.Clear();
		break;
	case 1:
		mid.Normal(input.getLvl(), input.getAvg(), false);
		break;
	case 2:
		mid.CentreOut(input.getLvl(), input.getAvg(), false);
		break;
	case 3:
		mid.Normal(input.getLvl(), input.getAvg());
		break;
	case 4:
		mid.CentreOut(input.getLvl(), input.getAvg());
		break;
	case 5:
		mid.FallingStar(input.getLvl(), input.getAvg(), true, 1);
		break;
	case 6:
		mid.Circle(input.getLvl(), input.getAvg(), true, 4);
		break;
	}
}