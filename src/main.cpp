//
// Created by enrico on 22.06.22.
//

#include "strip.hpp"
#include "micInput.hpp"


#define MICPIN A0
#define AVGPIN A1

#define LEDPIN 4
#define LEDCOUNT 34
#define BUTTONPIN 3

#define SUBLEDPIN 3
#define SUBLEDCOUNT 59
#define SUBBUTTONPIN 9

#define MIDLEDPIN1 2
#define MIDLEDPIN2 4
#define MIDLEDCOUNT 27
#define MIDBUTTONPIN 10

micinput input = micinput(MICPIN, AVGPIN, -100, 45, 30, 100);
strip<SUBLEDPIN, SUBLEDCOUNT> sub = strip<SUBLEDPIN, SUBLEDCOUNT>(false, 10, 1.5, 2.5);
strip<MIDLEDPIN1, MIDLEDCOUNT> mid1 = strip<MIDLEDPIN1, MIDLEDCOUNT>(true, 10, 1.5, 1.5);
strip<MIDLEDPIN2, MIDLEDCOUNT> mid2 = strip<MIDLEDPIN2, MIDLEDCOUNT>(false, 10, 1.5, 1.5);

int subState = 6;
int midState = 3;

long lastMillis = 0;
long loops = 0;

#define chechTime1 long f = millis();
#define chechTime2               \
	Serial.print("time millis:"); \
	Serial.println(millis() - f);

void checkCycles()
{
  	loops++;
	long currentMillis = millis();
	if (currentMillis - lastMillis > 1000)
	{
		Serial.print("Loops last second:");
		Serial.println(loops);

		lastMillis = currentMillis;
		loops = 0;
	}
}

void setup()
{
	Serial.begin(9600);
	input.init();

	sub.init();
	mid1.init();
	mid2.init();

	// sub.Test2();
	// delay(400);

	// mid1.Test2();
	// delay(400);

	// mid1.Test();
	// delay(400);

	// sub.Test();
	// delay(400);

	pinMode(SUBBUTTONPIN, INPUT);
	pinMode(MIDBUTTONPIN, INPUT);
}

void loop()
{
	chechTime1
	input.read();
	chechTime2

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
	// mid1.Normal(input.getLvl(), input.getAvg(), true, true);
	// sub.Normal(input.getMax(), 611, false, false);
	// mid1.Normal(input.getAvg(), 611, false, false);
	// mid1.Normal(input.getRaw(), input.getAvg(), true, true);
	// mid1.Normal(input.getLvl(), 611, false, false);

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
		sub.Circle(input.getLvl(), input.getAvg(), true, 7, 0.07);
		break;
	}

	// return;

	switch (midState)
	{
	case 0:
		mid1.Clear();
		mid2.Clear();
		break;
	case 1:
		mid1.Normal(input.getLvl(), input.getAvg(), false);
		mid2.Normal(input.getLvl(), input.getAvg(), false);
		break;
	case 2:
		mid1.CentreOut(input.getLvl(), input.getAvg(), false);
		mid2.CentreOut(input.getLvl(), input.getAvg(), false);
		break;
	case 3:
		mid1.Normal(input.getLvl(), input.getAvg());
		mid2.Normal(input.getLvl(), input.getAvg());
		break;
	case 4:
		mid1.CentreOut(input.getLvl(), input.getAvg());
		mid2.CentreOut(input.getLvl(), input.getAvg());
		break;
	case 5:
		mid1.FallingStar(input.getLvl(), input.getAvg(), true, 1);
		mid2.FallingStar(input.getLvl(), input.getAvg(), true, 1);
		break;
	case 6:
		mid1.Circle(input.getLvl(), input.getAvg(), true, 4);
		mid2.Circle(input.getLvl(), input.getAvg(), true, 4);
		break;
	}
	
	show();
	checkCycles();
}